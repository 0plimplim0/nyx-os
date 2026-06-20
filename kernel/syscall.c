// ============================================================
// syscall.c - Tabla de syscalls
// ============================================================
#include "kernel.h"

extern void* syscall_table[SYS_TABLE_SIZE];   // definido en kernel.c

void init_syscalls(void) {
    memset_asm(syscall_table, 0, sizeof(syscall_table));
}

void syscall_handler(uint32_t syscall_no, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg1; (void)arg2; (void)arg3; (void)arg4;
    printf("Syscall %d no implementada\n", syscall_no);
}

void* get_syscall_table(void) {
    return syscall_table;
}

void register_syscall(uint32_t num, void* handler) {
    if (num < SYS_TABLE_SIZE) syscall_table[num] = handler;
}