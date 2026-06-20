// ============================================================
// doom_utils.c - Implementaciones de funciones estándar para DOOM
// ============================================================
#include "fake_stdlib.h"
#include "kernel.h"

FILE *stderr = NULL;

// Funciones de memoria
void *memcpy(void *dest, const void *src, size_t n) {
    memcpy_asm(dest, src, n);
    return dest;
}

void *memset(void *s, int c, size_t n) {
    memset_asm(s, (uint8_t)c, n);
    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    return memcpy(dest, src, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        p1++; p2++;
    }
    return 0;
}

// Funciones de cadena
size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *orig = dest;
    while ((*dest++ = *src++) != '\0');
    return orig;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) dest[i] = src[i];
    for (; i < n; i++) dest[i] = '\0';
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcat(char *dest, const char *src) {
    char *orig = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++) != '\0');
    return orig;
}

char *strchr(const char *s, int c) {
    while (*s) { if (*s == (char)c) return (char*)s; s++; }
    return NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) { if (*s == (char)c) last = s; s++; }
    return (char*)last;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack, *n = needle;
        while (*h && *n && (*h == *n)) { h++; n++; }
        if (!*n) return (char*)haystack;
    }
    return NULL;
}

char *strpbrk(const char *s, const char *accept) {
    while (*s) {
        const char *a = accept;
        while (*a) { if (*s == *a) return (char*)s; a++; }
        s++;
    }
    return NULL;
}

size_t strcspn(const char *s, const char *reject) {
    size_t count = 0;
    while (*s) {
        const char *r = reject;
        int found = 0;
        while (*r) { if (*s == *r) { found = 1; break; } r++; }
        if (found) break;
        count++; s++;
    }
    return count;
}

size_t strspn(const char *s, const char *accept) {
    size_t count = 0;
    while (*s) {
        const char *a = accept;
        int found = 0;
        while (*a) { if (*s == *a) { found = 1; break; } a++; }
        if (!found) break;
        count++; s++;
    }
    return count;
}

char *strtok(char *str, const char *delim) {
    static char *next = NULL;
    if (str) next = str;
    if (!next) return NULL;
    while (*next && strchr(delim, *next)) next++;
    if (*next == '\0') return NULL;
    char *start = next;
    while (*next && !strchr(delim, *next)) next++;
    if (*next) *next++ = '\0';
    return start;
}

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *new = malloc(len);
    if (new) strcpy(new, s);
    return new;
}

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && ((*s1|0x20) == (*s2|0x20))) { s1++; s2++; }
    return (*(unsigned char *)s1|0x20) - (*(unsigned char *)s2|0x20);
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && *s2 && ((*s1|0x20) == (*s2|0x20))) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return (*(unsigned char *)s1|0x20) - (*(unsigned char *)s2|0x20);
}

// stdlib
void *malloc(size_t size) { (void)size; return NULL; }
void free(void *ptr) { (void)ptr; }
void abort(void) { while(1); }
int abs(int j) { return j < 0 ? -j : j; }
long int labs(long int j) { return j < 0 ? -j : j; }

int atoi(const char *s) {
    int sign = 1, result = 0;
    while (*s == ' ') s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') { result = result * 10 + (*s - '0'); s++; }
    return sign * result;
}

long int strtol(const char *s, char **end, int base) {
    long result = 0;
    int sign = 1;
    while (*s == ' ') s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;
    if ((base == 0 || base == 16) && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    }
    if (base == 0) base = 10;
    while (*s) {
        int digit;
        if (*s >= '0' && *s <= '9') digit = *s - '0';
        else if (*s >= 'a' && *s <= 'z') digit = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'Z') digit = *s - 'A' + 10;
        else break;
        if (digit >= base) break;
        result = result * base + digit;
        s++;
    }
    if (end) *end = (char*)s;
    return sign * result;
}

void exit(int status) { (void)status; while(1); }

// stdio
FILE *fopen(const char *path, const char *mode) { (void)path; (void)mode; return NULL; }
int fclose(FILE *stream) { (void)stream; return 0; }
size_t fread(void *ptr, size_t size, size_t count, FILE *stream) { (void)ptr; (void)size; (void)count; (void)stream; return 0; }
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream) { (void)ptr; (void)size; (void)count; (void)stream; return count; }
int fseek(FILE *stream, long offset, int whence) { (void)stream; (void)offset; (void)whence; return 0; }
long ftell(FILE *stream) { (void)stream; return 0; }
int feof(FILE *stream) { (void)stream; return 1; }
int ferror(FILE *stream) { (void)stream; return 0; }

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    // Usamos vprintf del kernel (que devuelve int)
    // Pero vprintf está en screen.c y devuelve int.
    // Llamamos a la función del kernel.
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}

int vprintf(const char *fmt, va_list args) {
    // Usamos la función del kernel (screen.c)
    return vprintf(fmt, args);
}

int snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    (void)buf; (void)size; (void)fmt; (void)args;
    return 0;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, 0x7FFFFFFF, fmt, args);
    va_end(args);
    return ret;
}

int fprintf(FILE *stream, const char *fmt, ...) {
    (void)stream;
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}

int vfprintf(FILE *stream, const char *fmt, va_list args) {
    (void)stream;
    return vprintf(fmt, args);
}

// Matemáticas
double sin(double x) {
    double result = 0;
    double term = x;
    int sign = 1;
    int n;
    for (n = 1; n <= 15; n += 2) {
        result += sign * term;
        sign = -sign;
        term = term * x * x / ((n+1)*(n+2));
    }
    return result;
}

double cos(double x) {
    double result = 1;
    double term = 1;
    int sign = -1;
    int n;
    for (n = 2; n <= 14; n += 2) {
        term = term * x * x / ((n-1)*n);
        result += sign * term;
        sign = -sign;
    }
    return result;
}

double sqrt(double x) {
    if (x == 0) return 0;
    double guess = x / 2;
    double prev;
    do {
        prev = guess;
        guess = (guess + x / guess) / 2;
    } while (guess != prev);
    return guess;
}

double fabs(double x) { return x < 0 ? -x : x; }
double pow(double x, double y) { (void)x; (void)y; return 0; }
double floor(double x) { return (double)(int)x; }
double ceil(double x) { return (double)((int)x + (x > 0 ? 1 : 0)); }