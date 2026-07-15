#include "libc.h"

/* A large-.bss stdin consumer for pstorm's pipeline reproducer. Reads all of stdin
 * to EOF, then exits — non-interactive (unlike less), so a `producer | bigread`
 * pipeline auto-completes without a keypress. The big .bss makes execve() slow,
 * matching the enlarged less.elf that reliably reproduced the residual shared-libc
 * fault: a slow execve widens the timing window for the fault. */
static char sink[176 * 1024];   /* ~176 KB .bss — same magnitude as the enlarged less */

int main(void) {
    long n, total = 0;
    while ((n = read(0, sink, sizeof(sink))) > 0) total += n;
    return (int)(total & 0x7f);
}
