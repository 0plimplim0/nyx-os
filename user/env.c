#include "libc.h"

/* env — print the process environment, one NAME=VALUE per line. The environment is
 * inherited from the parent through execve's envp vector, which crt0 publishes as
 * the libc `environ` global. Run from the shell, it shows the shell's exported vars
 * (USER, HOME, PATH, ... plus anything `export`ed). */

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    if (!environ) return 0;
    for (char** e = environ; *e; e++)
        printf("%s\n", *e);
    return 0;
}
