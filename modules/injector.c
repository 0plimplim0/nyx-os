// ============================================================
// modules/injector.c - Inyección de código (v2)
// ============================================================
#include "modules.h"

int inject_so(pid_t target_pid, const char* so_path) {
    process_t* target = find_process(target_pid);
    if (!target) return -1;

    target->state = 1;
    void* remote_addr = allocate_remote_memory(target, 4096);
    if (!remote_addr) { target->state = 0; return -1; }

    write_remote_memory(target, remote_addr, so_path, strlen(so_path)+1);

    uint32_t dlopen_addr = get_kernel_symbol("dlopen");
    remote_syscall(target, dlopen_addr, remote_addr, (void*)RTLD_NOW);

    target->state = 0;
    return 0;
}

int inject_shellcode(pid_t target_pid, void* shellcode, size_t len) {
    process_t* target = find_process(target_pid);
    if (!target) return -1;

    target->state = 1;
    void* exec_addr = allocate_remote_memory(target, len + 1024);
    if (!exec_addr) { target->state = 0; return -1; }
    write_remote_memory(target, exec_addr, shellcode, len);

    uint32_t create_thread_addr = get_kernel_symbol("kernel_thread_create");
    remote_syscall(target, create_thread_addr, exec_addr, NULL);

    target->state = 0;
    return 0;
}

void* module_injector_init(void) {
    register_syscall(NYX_INJECT_SO, inject_so);
    register_syscall(NYX_INJECT_SC, inject_shellcode);
    return NULL;
}