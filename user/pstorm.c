#include "libc.h"

/* pstorm — pipeline storm: reproduce the residual shared-libc fault by driving the
 * exact fork+pipe+execve churn of a `producer | consumer` pipeline in a tight loop,
 * WITHOUT needing the shell to parse a `|` (which the ES keyboard layout makes hard
 * to type under QEMU). Each iteration forks /ls.elf with stdout on the pipe and a
 * large-.bss /bigread.elf with stdin on the pipe, then reaps both. A stage that
 * faults on a not-present shared-libc page is killed by the kernel (v5.8.44) with
 * waitpid status 128+SIGSEGV = 139, surfaced here and in the [fault] serial line.
 *   usage: pstorm [iterations]   (default 60) */
int main(int argc, char** argv) {
    int iters = 60;
    if (argc >= 2) { int v = atoi(argv[1]); if (v > 0) iters = v; }

    int faults = 0;
    for (int i = 0; i < iters; i++) {
        int fds[2];
        if (pipe(fds) != 0) { printf("pstorm: pipe() failed at %d\n", i); return 1; }

        long p1 = fork();
        if (p1 == 0) {                          /* producer -> /ls.elf */
            dup2(fds[1], 1);
            close(fds[0]); close(fds[1]);
            char* av[] = { "/ls.elf", 0 };
            execve("/ls.elf", av, 0);
            exit(127);
        }
        long p2 = fork();
        if (p2 == 0) {                          /* consumer -> /bigread.elf */
            dup2(fds[0], 0);
            close(fds[0]); close(fds[1]);
            char* av[] = { "/bigread.elf", 0 };
            execve("/bigread.elf", av, 0);
            exit(127);
        }
        close(fds[0]); close(fds[1]);           /* parent keeps neither end */

        int st1 = 0, st2 = 0;
        waitpid((int)p1, &st1);
        waitpid((int)p2, &st2);
        int hit = (st1 == 139 || st2 == 139);
        if (hit) faults++;
        printf("[%02d] ls=%d bigread=%d%s\n", i, st1, st2, hit ? "  <-- FAULT" : "");
    }
    printf("pstorm: %d iterations, %d faulted\n", iters, faults);
    return 0;
}
