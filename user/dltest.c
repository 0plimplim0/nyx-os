#include "libc.h"

/* dltest — exercises NyxOS runtime dynamic loading. It does NOT link against
 * libdemo; it loads it at runtime with dlopen(), resolves functions by name with
 * dlsym(), and calls them through the returned pointers. (printf itself comes
 * from the shared libc, mapped at boot.) */

int main(int argc, char** argv) {
    (void)argc; (void)argv;

    long h = dlopen("/libdemo.so");
    if (h < 0) { printf("dltest: dlopen(/libdemo.so) failed\n"); return 1; }
    printf("dltest: dlopen(/libdemo.so) -> handle %ld\n", h);

    int  (*gcd)(int, int)  = (int (*)(int, int)) dlsym(h, "demo_gcd");
    long (*fib)(int)       = (long (*)(int))     dlsym(h, "demo_fib");
    const char* (*nm)(void)= (const char* (*)(void)) dlsym(h, "demo_name");

    if (!gcd || !fib || !nm) { printf("dltest: dlsym failed (%p %p %p)\n", gcd, fib, nm); return 1; }

    printf("dltest: demo_name() = %s\n", nm());
    printf("dltest: demo_gcd(48,36) = %d (expect 12)\n", gcd(48, 36));
    printf("dltest: demo_fib(10)    = %ld (expect 55)\n", fib(10));
    printf("dltest: OK\n");
    return 0;
}
