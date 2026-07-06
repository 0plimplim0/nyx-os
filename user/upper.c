#include "libc.h"

/* upper — read stdin (fd 0) until EOF, uppercase it, write to stdout. The
 * classic pipeline consumer: the shell dup2()s a pipe's read end onto fd 0, and
 * read() blocks until the producer writes (EOF once every writer has closed). */
int main(void) {
    char buf[128];
    long n;
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        for (long i = 0; i < n; i++)
            if (buf[i] >= 'a' && buf[i] <= 'z') buf[i] -= 32;
        write(1, buf, n);
    }
    return 0;
}
