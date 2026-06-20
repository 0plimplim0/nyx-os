// ============================================================
// process.c - Gestor de procesos básico con tabla estática
// ============================================================
#include "kernel.h"

extern process_t* process_table[MAX_PROCESSES];   // definido en kernel.c
extern int process_count;                         // definido en kernel.c
static uint32_t next_pid = 1;

void init_process(void) {
    memset_asm(process_table, 0, sizeof(process_table));
    process_t* init = create_process("init", NULL, 0);
    init->state = 0;
    process_count = 1;
}

process_t* create_process(const char* name, void* entry, uint32_t flags) {
    (void)entry;
    if (process_count >= MAX_PROCESSES) return NULL;
    process_t* p = (process_t*)kmalloc(sizeof(process_t));
    memset_asm(p, 0, sizeof(process_t));
    p->pid = next_pid++;
    p->ppid = 0;
    p->uid = 0;
    p->euid = 0;
    p->state = 0;
    p->stealth_level = (flags & 0x1) ? 5 : 0;
    strncpy(p->comm, name, 31);
    p->comm[31] = '\0';
    process_table[process_count++] = p;
    return p;
}

void destroy_process(uint32_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i]->pid == pid) {
            kfree(process_table[i]);
            process_table[i] = NULL;
            return;
        }
    }
}

process_t* find_process(uint32_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->pid == pid)
            return process_table[i];
    }
    return NULL;
}

process_t* get_current_process(void) {
    return process_table[0];
}

void hide_process(uint32_t pid) {
    process_t* p = find_process(pid);
    if (p) p->stealth_level = 5;
}

void unhide_process(uint32_t pid) {
    process_t* p = find_process(pid);
    if (p) p->stealth_level = 0;
}

void schedule(void) {
    // No implementado
}

void switch_to_task(process_t* next) {
    (void)next;
}