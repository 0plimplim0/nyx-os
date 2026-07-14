#include "libc.h"

/* less / more — a full-screen pager. It reads a file (given as argv[1]) or, with no
 * argument, standard input (so `cmd | less` works), buffers it, and displays it a
 * screenful at a time on the terminal's cursor-addressed "screen mode" (the same one
 * edit/top use: ESC[2J to clear, ESC[r;cH to position). Navigation keys come from
 * readkey() — which reads the KEYBOARD directly, independent of fd 0 — so paging a
 * pipe still works even though stdin is the pipe, not the terminal.
 *
 *   Space / f / PgDn        page down        j / Down / Enter   line down
 *   b / PgUp                page up          k / Up             line up
 *   g / Home                top              G / End            bottom
 *   q / ESC                 quit
 *
 * Buffers are .bss statics: read() copies into them from the kernel and so cannot
 * fault an unfaulted lazy-heap page. */

#define K_UP    0x80
#define K_DOWN  0x81
#define K_HOME  0x84
#define K_END   0x85
#define K_PGUP  0x86
#define K_PGDN  0x87

#define TEXTROWS 22          /* text rows; the status bar sits on row TEXTROWS+1 */
#define SCREEN_W 80
#define MAXLINES 2048

/* Keep the .bss modest. A big program image makes execve() slow, which widens the
 * window for a transient IRQ fault in the forked shell child of a `cmd | less` pipe
 * stage — so buffer 32 KB, plenty for paging the files/output on this system. */
static char content[32768];        /* raw file / stdin bytes */
static int  line_off[MAXLINES];    /* byte offset of each line's start in content */
static int  nlines;
static char scr[8192];             /* one whole frame, emitted in a single write() */
static int  sp;
static char name[80];              /* what we're paging, for the status bar */

static void s_putc(char c) { if (sp < (int)sizeof(scr) - 1) scr[sp++] = c; }
static void s_str(const char* s) { while (*s) s_putc(*s++); }
static void s_int(int v) {
    char t[12]; int n = 0;
    if (v < 0) { s_putc('-'); v = -v; }
    if (v == 0) t[n++] = '0';
    while (v) { t[n++] = (char)('0' + v % 10); v /= 10; }
    while (n) s_putc(t[--n]);
}
static void s_goto(int row, int col) { s_str("\x1b["); s_int(row); s_putc(';'); s_int(col); s_putc('H'); }

/* Split the loaded content into lines (offsets), turning '\n' (and '\r') into '\0'. */
static void split_lines(int total) {
    nlines = 0;
    int start = 0;
    for (int i = 0; i < total && nlines < MAXLINES; i++) {
        if (content[i] == '\n') {
            content[i] = '\0';
            line_off[nlines++] = start;
            start = i + 1;
        } else if (content[i] == '\r') {
            content[i] = '\0';                  /* strip CR so CRLF renders cleanly */
        }
    }
    if (start < total && nlines < MAXLINES) line_off[nlines++] = start;   /* last, no NL */
    if (nlines == 0) { content[0] = '\0'; line_off[0] = 0; nlines = 1; }
}

static void render(int top) {
    sp = 0;
    s_str("\x1b[2J");                            /* clear -> screen mode */
    for (int r = 0; r < TEXTROWS; r++) {
        s_goto(r + 1, 1);
        int fl = top + r;
        if (fl < nlines) {
            char* L = content + line_off[fl];
            for (int c = 0; c < SCREEN_W && L[c]; c++) s_putc(L[c]);
        } else {
            s_putc('~');                          /* past EOF, like vi/less */
        }
    }
    /* status bar — black on light-grey (SGR 30;47); ESC[K paints the whole row */
    int last = top + TEXTROWS; if (last > nlines) last = nlines;
    int pct = nlines ? (last * 100 / nlines) : 100;
    s_goto(TEXTROWS + 1, 1);
    s_str("\x1b[30;47m\x1b[K ");
    s_str(name);
    s_str("  ln "); s_int(top + 1); s_putc('-'); s_int(last); s_putc('/'); s_int(nlines);
    s_str("  "); s_int(pct); s_putc('%');
    s_str("   [Space/b page  j/k line  g/G top/bot  q quit]");
    s_str("\x1b[0m");
    s_goto(TEXTROWS + 1, 1);                      /* park the cursor on the status bar */
    write(1, scr, sp);
}

int main(int argc, char** argv) {
    long total = 0, r;
    if (argc >= 2) {
        long fd = open(argv[1], O_RDONLY, 0);
        if (fd < 0) { printf("less: %s: cannot open\n", argv[1]); return 1; }
        while (total < (long)sizeof(content) - 1 &&
               (r = read((int)fd, content + total, sizeof(content) - 1 - total)) > 0)
            total += r;
        close((int)fd);
        strncpy(name, argv[1], sizeof(name) - 1);
    } else {
        while (total < (long)sizeof(content) - 1 &&
               (r = read(0, content + total, sizeof(content) - 1 - total)) > 0)
            total += r;
        strcpy(name, "(stdin)");
    }
    content[total] = '\0';
    split_lines((int)total);

    int maxtop = nlines - TEXTROWS; if (maxtop < 0) maxtop = 0;

    ttymode(TTY_RAW);
    int top = 0;
    for (;;) {
        render(top);
        long k = readkey(0);                      /* block for a navigation key */
        if (k <= 0) continue;
        if (k == 'q' || k == 'Q' || k == 0x1B) break;
        else if (k == ' ' || k == 'f' || k == K_PGDN)              top += TEXTROWS;
        else if (k == 'b' || k == K_PGUP)                          top -= TEXTROWS;
        else if (k == 'j' || k == '\n' || k == '\r' || k == K_DOWN) top += 1;
        else if (k == 'k' || k == K_UP)                            top -= 1;
        else if (k == 'g' || k == K_HOME)                          top = 0;
        else if (k == 'G' || k == K_END)                           top = maxtop;
        if (top > maxtop) top = maxtop;
        if (top < 0) top = 0;
    }
    ttymode(TTY_CANON);
    printf("\x1b[2J\x1b[H");                       /* clear on exit; back to scrollback */
    return 0;
}
