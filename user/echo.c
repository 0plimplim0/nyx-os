#include "libc.h"

/* echo — write the arguments, space-separated, to stdout (fd 1). Inside a
 * pipeline the shell dup2()s a pipe onto fd 1, so this feeds the next stage;
 * standalone it falls back to the console. */
int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) write(1, " ", 1);
        write(1, argv[i], strlen(argv[i]));
    }
    write(1, "\n", 1);
    return 0;
}
