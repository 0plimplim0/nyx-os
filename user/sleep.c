#include "libc.h"

/* sleep N — pause for N seconds. sleep_sec (SYS_SLEEP) blocks on the kernel's timer
 * wait queue, so the process sleeps efficiently while the scheduler runs others; a
 * signal (e.g. Ctrl-C) cuts it short. Integer seconds only. */

int main(int argc, char** argv) {
    if (argc < 2) { printf("usage: sleep SECONDS\n"); return 1; }
    int secs = atoi(argv[1]);
    if (secs < 0) secs = 0;
    sleep_sec(secs);
    return 0;
}
