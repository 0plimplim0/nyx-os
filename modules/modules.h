#ifndef MODULES_H
#define MODULES_H

#include "../kernel/kernel.h"

// Constantes de archivo
#define O_RDONLY    0x0000
#define O_WRONLY    0x0001
#define O_RDWR      0x0002
#define O_CREAT     0x0200
#define O_APPEND    0x0400

// Constantes de red
#define SYN_FLAG    0x02
#define RTLD_NOW    0x0002

// Syscalls personalizadas
#define NYX_INJECT_SO  512
#define NYX_INJECT_SC  513

// Variables globales del kernel (extern)
extern process_t* process_table[];
extern int process_count;
extern void* syscall_table[];

// Funciones del kernel (declaraciones)
extern int vfs_open(const char*, int, mode_t);
extern int vfs_read(int, void*, size_t);
extern int vfs_write(int, const void*, size_t);
extern int vfs_close(int);
extern void hide_process(uint32_t);
extern process_t* find_process(uint32_t);
extern void* allocate_remote_memory(process_t*, size_t);
extern void write_remote_memory(process_t*, void*, const void*, size_t);
extern uint32_t get_kernel_symbol(const char*);
extern void remote_syscall(process_t*, uint32_t, void*, void*);
extern void hide_file(const char*);
extern void hook_proc_net_tcp(void*);
extern void restore_syscalls(void);
extern void kernel_thread_create(const char*, void (*)(void), void*);
extern void register_syscall(uint32_t, void*);
extern void init_raw_socket(void);
extern int net_create_raw_socket(void);
extern int net_connect(int, uint32_t, uint16_t);
extern int net_bind(int, uint32_t, uint16_t);
extern int net_listen(int, int);
extern int net_accept(int);
extern int net_recv(int, void*, size_t);
extern int net_close(int);
extern int net_send_raw(void*, size_t);
extern uint32_t random_ip(void);
extern int wait_for_ack(uint32_t, uint16_t, int);
extern void log_open_port(uint32_t, uint16_t);
extern void send_rst(uint32_t, uint16_t);
extern void set_monitor_mode(const char*);
extern void send_wps_pin(const char*, const char*, const char*);
extern int wait_for_wps_success(const char*, int);
extern void log_wps_pin(const char*, const char*);
extern void send_deauth_packet(const char*, const char*, const char*);
extern void load_wifi_firmware(void);
extern void* hook_irq(int, void (*)(void));
extern char scancode_to_ascii(uint8_t);
extern process_t* get_current_process(void);
extern uint32_t tick_count;

// ============================================================
// FUNCIONES AUXILIARES INLINE (no declaradas en kernel.h)
// ============================================================

// sscanf básico (solo %s, %d, %u, y %hu con 'h')
static inline int sscanf(const char *str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int matches = 0;
    const char *s = str;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 's') {
                char *buf = va_arg(args, char*);
                while (*s && *s != ' ' && *s != '\t' && *s != '\n') *buf++ = *s++;
                *buf = '\0';
                matches++;
                fmt++;
            } else if (*fmt == 'd') {
                int *val = va_arg(args, int*);
                int sign = 1, num = 0;
                while (*s == ' ') s++;
                if (*s == '-') { sign = -1; s++; }
                while (*s >= '0' && *s <= '9') { num = num*10 + (*s-'0'); s++; }
                *val = sign * num;
                matches++;
                fmt++;
            } else if (*fmt == 'u') {
                unsigned int *val = va_arg(args, unsigned int*);
                unsigned int num = 0;
                while (*s == ' ') s++;
                while (*s >= '0' && *s <= '9') { num = num*10 + (*s-'0'); s++; }
                *val = num;
                matches++;
                fmt++;
            } else if (*fmt == 'h') {
                fmt++; // saltar 'h'
                if (*fmt == 'u') {
                    unsigned short *val = va_arg(args, unsigned short*);
                    unsigned int num = 0;
                    while (*s == ' ') s++;
                    while (*s >= '0' && *s <= '9') { num = num*10 + (*s-'0'); s++; }
                    *val = (unsigned short)num;
                    matches++;
                    fmt++;
                }
            } else {
                fmt++;
            }
        } else {
            if (*fmt == ' ') while (*s == ' ') s++;
            else if (*fmt == *s) s++;
            else break;
            fmt++;
        }
    }
    va_end(args);
    return matches;
}

// inet_addr – convierte IP "a.b.c.d" a uint32_t
static inline uint32_t inet_addr(const char *ip) {
    uint32_t addr = 0;
    uint8_t parts[4] = {0,0,0,0};
    int i = 0;
    while (*ip && i < 4) {
        uint32_t val = 0;
        while (*ip >= '0' && *ip <= '9') { val = val*10 + (*ip-'0'); ip++; }
        parts[i++] = (uint8_t)val;
        if (*ip == '.') ip++;
    }
    addr = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    return addr;
}

// ntohs – intercambio de bytes para red a host (little-endian)
static inline uint16_t ntohs(uint16_t x) {
    return (x >> 8) | (x << 8);
}

// inet_sk – stub (evita error de compilación)
static inline void* inet_sk(void *sk) { return sk; }

#endif // MODULES_H