#include "libc.h"

/* sh — the NyxOS userspace shell. Runs entirely in ring 3 on the Unix process
 * toolkit the kernel grew in v5.7.25..v5.8.6: fork() + execve(argv) + waitpid()
 * + pipe() + dup2(). A command line is up to MAX_STAGES programs joined by '|';
 * each stage runs as a fork+execve'd child with its stdin/stdout wired to the
 * neighbouring pipes, and the shell waitpid()s them all.
 *
 * Modes: `sh -c "line"` runs one line; with no arguments it runs a built-in
 * demo script (interactive stdin needs kernel keyboard routing — future work). */

#define MAX_STAGES 4
#define MAX_ARGS   8

/* Trim leading/trailing spaces in place. */
static char* trim(char* s) {
    while (*s == ' ') s++;
    char* e = s + strlen(s);
    while (e > s && e[-1] == ' ') *--e = '\0';
    return s;
}

/* Split a stage ("prog arg1 arg2") into an argv[]; modifies s. Returns argc. */
static int split_args(char* s, char** av, int max) {
    int n = 0;
    while (*s && n < max - 1) {
        while (*s == ' ') *s++ = '\0';
        if (!*s) break;
        av[n++] = s;
        while (*s && *s != ' ') s++;
    }
    av[n] = 0;
    return n;
}

/* Resolve a command name: "echo" -> "/echo.elf"; anything already containing a
 * '/' or '.' is taken as an explicit path. */
static void resolve(const char* name, char* out) {
    if (strchr(name, '/') || strchr(name, '.')) { strcpy(out, name); return; }
    out[0] = '/';
    strcpy(out + 1, name);
    strcat(out, ".elf");
}

/* Run one command line (destroys it). Stages split on '|', each fork+execve'd
 * with pipes wired via dup2; the shell reaps every stage with waitpid. */
static void run_line(char* line) {
    char* stages[MAX_STAGES];
    int nstages = 0;
    char* p = line;
    stages[nstages++] = p;
    while ((p = strchr(p, '|')) != 0 && nstages < MAX_STAGES) {
        *p++ = '\0';
        stages[nstages++] = p;
    }

    long pids[MAX_STAGES];
    int prev_rd = -1;                          /* read end of the previous pipe */
    for (int s = 0; s < nstages; s++) {
        int has_next = (s < nstages - 1);
        int pfd[2] = { -1, -1 };
        if (has_next && pipe(pfd) != 0) { printf("sh: pipe failed\n"); return; }

        long pid = fork();
        if (pid == 0) {
            /* Child: stdin from the previous pipe, stdout into the next one,
             * then drop the raw fds (the dup took a reference) and become the
             * program. If execve returns, the program wasn't found. */
            if (prev_rd >= 0) { dup2(prev_rd, 0); close(prev_rd); }
            if (has_next)     { close(pfd[0]); dup2(pfd[1], 1); close(pfd[1]); }
            char* av[MAX_ARGS];
            int ac = split_args(trim(stages[s]), av, MAX_ARGS);
            if (ac == 0) exit(0);
            char path[64];
            resolve(av[0], path);
            execve(path, av, 0);
            printf("sh: %s: not found\n", path);
            exit(127);
        } else if (pid < 0) {
            printf("sh: fork failed\n");
            return;
        }
        pids[s] = pid;
        /* Parent: close the ends now owned by the children, keep the new pipe's
         * read end for the next stage. Closing promptly is what makes EOF fire
         * once the last writer exits. */
        if (prev_rd >= 0) close(prev_rd);
        if (has_next) { close(pfd[1]); prev_rd = pfd[0]; }
    }

    for (int s = 0; s < nstages; s++) {
        int st = 0;
        waitpid((int)pids[s], &st);
        if (st != 0)
            printf("sh: [pid %d] exited with status %d\n", (int)pids[s], st);
    }
}

int main(int argc, char** argv) {
    /* sh -c "one command line" */
    if (argc >= 3 && strcmp(argv[1], "-c") == 0) {
        char buf[128];
        strncpy(buf, argv[2], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        run_line(buf);
        return 0;
    }

    /* Script mode: a canned demo — one plain command, one two-stage pipeline,
     * and one argv+status check. Every line exercises fork/execve/waitpid, the
     * pipeline adds pipe+dup2. */
    static const char* script[] = {
        "echo hello from the NyxOS userspace shell",
        "echo pipelines are working on nyxos | upper",
        "args one two three",
    };
    printf("NyxOS sh v0.1 — fork+execve+waitpid+pipe+dup2, all in ring 3\n");
    for (unsigned i = 0; i < sizeof(script) / sizeof(script[0]); i++) {
        char buf[128];
        strncpy(buf, script[i], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        printf("sh$ %s\n", script[i]);
        run_line(buf);
    }
    printf("sh: script done.\n");
    return 0;
}
