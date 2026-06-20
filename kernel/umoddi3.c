// umoddi3.c - Rutinas de soporte para GCC (mínimo)
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;

uint64_t __umoddi3(uint64_t a, uint64_t b) {
    // División simple por resta (no eficiente, pero funciona)
    if (b == 0) return 0;
    uint64_t rem = a;
    while (rem >= b) rem -= b;
    return rem;
}

uint64_t __udivdi3(uint64_t a, uint64_t b) {
    if (b == 0) return 0;
    uint64_t quot = 0;
    uint64_t rem = a;
    while (rem >= b) { rem -= b; quot++; }
    return quot;
}