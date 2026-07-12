#include "libc.h"

/* date — print the current date and time from the real-time clock. time() (SYS_TIME)
 * reads the RTC into a broken-down nyx_tm; we format it ISO-style as
 * "YYYY-MM-DD HH:MM:SS". */

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    nyx_tm t;
    if (time(&t) != 0) { printf("date: cannot read the clock\n"); return 1; }
    printf("%04d-%02d-%02d %02d:%02d:%02d\n",
           t.year, t.mon, t.mday, t.hour, t.min, t.sec);
    return 0;
}
