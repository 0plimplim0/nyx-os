#include "libc.h"

/* sockdemo — userspace TCP sockets (v5.8.52). Connects to the kernel's built-in
 * loopback echo service on 127.0.0.1:7, sends a line, reads the echo back, and
 * prints it. Exercises the real path: socket() -> connect() (drives the 3-way
 * handshake) -> write() (tcp_send) -> read() (blocks on tcp_recv) -> close().
 * Fully self-contained over loopback — no NIC or external host needed. */

int main(void) {
    printf("sockdemo: opening a TCP socket...\n");
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { printf("sockdemo: socket() failed\n"); return 1; }

    printf("sockdemo: connecting to 127.0.0.1:7 (echo)...\n");
    if (connect(fd, inet_ipv4(127, 0, 0, 1), 7) != 0) {
        printf("sockdemo: connect() failed\n");
        close(fd);
        return 1;
    }
    printf("sockdemo: connected (fd=%d)\n", fd);

    const char* msg = "hello from userspace sockets!";
    int mlen = strlen(msg);
    if (write(fd, msg, mlen) != mlen) {
        printf("sockdemo: write() failed\n");
        close(fd);
        return 1;
    }
    printf("sockdemo: sent %d bytes: \"%s\"\n", mlen, msg);

    char buf[128];
    int n = read(fd, buf, sizeof(buf) - 1);   /* blocks until the echo arrives */
    if (n <= 0) {
        printf("sockdemo: no echo (read returned %d)\n", n);
        close(fd);
        return 1;
    }
    buf[n] = '\0';
    printf("sockdemo: got echo (%d bytes): \"%s\"\n", n, buf);

    close(fd);
    int ok = (n == mlen);
    printf("sockdemo: %s\n", ok ? "OK -- echo matched, sockets work!" : "MISMATCH");
    return ok ? 0 : 1;
}
