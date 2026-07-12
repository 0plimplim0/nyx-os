/* libdemo — a tiny demonstration shared library for NyxOS's dlopen/dlsym.
 *
 * It is prelinked at a fixed VA (0x31000000, see the makefile) and loaded on
 * demand by dlopen("/libdemo.so"); a program resolves these functions by name
 * with dlsym() at runtime. Self-contained (no libc dependency) so it needs no
 * relocation and no symbols beyond its own. */

int demo_gcd(int a, int b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { int t = b; b = a % b; a = t; }
    return a;
}

int demo_pow(int base, int exp) {
    int r = 1;
    while (exp-- > 0) r *= base;
    return r;
}

long demo_fib(int n) {
    long a = 0, b = 1;
    while (n-- > 0) { long t = a + b; a = b; b = t; }
    return a;
}

const char* demo_name(void) {
    return "libdemo v1 (dlopen'd)";
}
