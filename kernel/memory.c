// ============================================================
// memory.c - Asignador de páginas y heap básico
// ============================================================
#include "kernel.h"

static uint32_t memory_available = 0;
static uint32_t* page_stack = NULL;
static uint32_t page_stack_top = 0;

void init_memory(uint32_t mem_size) {
    memory_total = mem_size;
    memory_used = 0;
    memory_available = mem_size;
    uint32_t num_pages = mem_size / PAGE_SIZE;
    page_stack = (uint32_t*)KERNEL_HEAP_START;
    page_stack_top = 0;
    for (uint32_t i = 1; i < num_pages; i++) {
        page_stack[page_stack_top++] = i * PAGE_SIZE;
    }
}

void* alloc_page(void) {
    if (page_stack_top == 0) return NULL;
    uint32_t addr = page_stack[--page_stack_top];
    memory_used += PAGE_SIZE;
    memory_available -= PAGE_SIZE;
    return (void*)addr;
}

void free_page(void* addr) {
    page_stack[page_stack_top++] = (uint32_t)addr;
    memory_used -= PAGE_SIZE;
    memory_available += PAGE_SIZE;
}