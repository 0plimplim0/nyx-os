#include "kernel.h"

extern void* syscall_table[SYS_TABLE_SIZE];

void init_syscalls(void) {
    memset_asm(syscall_table, 0, sizeof(syscall_table));
}

uint32_t syscall_handler_c(uint32_t no, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    (void)a4; (void)a5;
    switch (no) {
        case 0: {  // exit(status)
            extern process_t* get_current_process(void);
            printf("[USER] exit(%u)\n", a1);
            process_t* cur = get_current_process();
            if (cur) cur->state = 0;
            // Wait for timer to schedule another process
            __asm__ volatile("sti; hlt");
            return 0;
        }
        case 1: {  // write(fd, buf, len)
            int fd = (int)a1;
            const char* buf = (const char*)a2;
            int len = (int)a3;
            if (fd == 1 || fd == 2) {  // stdout/stderr
                for (int i = 0; i < len; i++) putchar(buf[i]);
            }
            return len;
        }
        case 2: {  // print string
            printf("%s", (const char*)a1);
            return 0;
        }
        default:
            printf("[SYSCALL] Unknown syscall %u\n", no);
            return -1;
    }
}

void* get_syscall_table(void) {
    return syscall_table;
}

void register_syscall(uint32_t num, void* handler) {
    if (num < SYS_TABLE_SIZE) syscall_table[num] = handler;
}
