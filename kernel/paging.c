// ============================================================
// paging.c - Gestor de paginación (sin far jump)
// ============================================================
#include "kernel.h"

static uint32_t* current_page_directory = NULL;
static uint32_t* kernel_page_directory = NULL;

void init_paging(void) {
    printf("[PAGING] Allocating page directory...\n");
    kernel_page_directory = (uint32_t*)alloc_page();
    if (!kernel_page_directory) kernel_panic("No page directory");
    memset_asm(kernel_page_directory, 0, PAGE_SIZE);
    printf("[PAGING] Page directory at %x\n", kernel_page_directory);

    printf("[PAGING] Allocating page table (identity map 0-4MB)...\n");
    uint32_t* page_table = (uint32_t*)alloc_page();
    if (!page_table) kernel_panic("No page table");
    for (uint32_t i = 0; i < 1024; i++) {
        page_table[i] = (i * PAGE_SIZE) | 3;
    }
    kernel_page_directory[0] = (uint32_t)page_table | 3;

    printf("[PAGING] Loading CR3 with %x\n", kernel_page_directory);
    write_cr3((uint32_t)kernel_page_directory);

    printf("[PAGING] Enabling paging...\n");
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

    // Forzar un salto corto para vaciar la cola de prebúsqueda
    __asm__ volatile("jmp 1f\n1:");

    current_page_directory = kernel_page_directory;
    printf("[PAGING] Enabled successfully.\n");
}

void* get_phys_addr(void* virtual_addr) {
    uint32_t pd_index = (uint32_t)virtual_addr >> 22;
    uint32_t pt_index = ((uint32_t)virtual_addr >> 12) & 0x3FF;
    uint32_t offset = (uint32_t)virtual_addr & 0xFFF;
    if (!(kernel_page_directory[pd_index] & 1)) return NULL;
    uint32_t* pt = (uint32_t*)(kernel_page_directory[pd_index] & ~0xFFF);
    if (!(pt[pt_index] & 1)) return NULL;
    return (void*)((pt[pt_index] & ~0xFFF) + offset);
}

void map_page(void* phys, void* virt, uint32_t flags) { (void)phys; (void)virt; (void)flags; }
void unmap_page(void* virt) { (void)virt; }

void* clone_page_directory(void) {
    return NULL;
}