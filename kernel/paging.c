#include "kernel.h"

#define IDENTITY_MAP_MB 64

static uint32_t* current_page_directory = NULL;
static uint32_t* kernel_page_directory = NULL;

void init_paging(void) {
    printf("[PAGING] Allocating page directory...\n");
    kernel_page_directory = (uint32_t*)alloc_page();
    if (!kernel_page_directory) kernel_panic("No page directory");
    memset_asm(kernel_page_directory, 0, PAGE_SIZE);
    printf("[PAGING] Page directory at %x\n", kernel_page_directory);

    int num_tables = IDENTITY_MAP_MB / 4;
    printf("[PAGING] Identity-mapping %d MB (%d page tables)...\n", IDENTITY_MAP_MB, num_tables);
    for (int t = 0; t < num_tables; t++) {
        uint32_t* pt = (uint32_t*)alloc_page();
        if (!pt) kernel_panic("No page table");
        for (uint32_t i = 0; i < 1024; i++) {
            pt[i] = ((t * 0x400000) + (i * PAGE_SIZE)) | 3;
        }
        kernel_page_directory[t] = (uint32_t)pt | 3;
    }

    printf("[PAGING] Loading CR3 with %x\n", kernel_page_directory);
    write_cr3((uint32_t)kernel_page_directory);

    printf("[PAGING] Enabling paging...\n");
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
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

void map_page(void* phys, void* virt, uint32_t flags) {
    (void)phys; (void)virt; (void)flags;
    map_page_dir(kernel_page_directory, phys, virt, flags);
}

void unmap_page(void* virt) {
    uint32_t pd_idx = (uint32_t)virt >> 22;
    uint32_t pt_idx = ((uint32_t)virt >> 12) & 0x3FF;
    if (!(kernel_page_directory[pd_idx] & 1)) return;
    uint32_t* pt = (uint32_t*)(kernel_page_directory[pd_idx] & ~0xFFF);
    pt[pt_idx] = 0;
    __asm__ volatile("invlpg (%0)" :: "r"(virt));
}

uint32_t* get_kernel_page_directory(void) {
    return kernel_page_directory;
}

void switch_page_directory(uint32_t* pd) {
    if (!pd) pd = kernel_page_directory;
    current_page_directory = pd;
    write_cr3((uint32_t)pd);
}

uint32_t* alloc_page_directory(void) {
    uint32_t* pd = (uint32_t*)alloc_page();
    if (!pd) return NULL;
    memset_asm(pd, 0, PAGE_SIZE);

    int num_tables = IDENTITY_MAP_MB / 4;
    for (int t = 0; t < num_tables; t++) {
        // Clone the page table (kernel-owned pages stay supervisor-only)
        uint32_t* src_pt = (uint32_t*)(kernel_page_directory[t] & ~0xFFF);
        if (!src_pt) continue;
        uint32_t* pt = (uint32_t*)alloc_page();
        if (!pt) continue;
        // Copy page table entries, keep supervisor flag (bit 2 = 0)
        for (uint32_t i = 0; i < 1024; i++) {
            uint32_t entry = src_pt[i];
            if (entry & 1) {
                // Keep present, writable, but clear user bit (bit 2)
                pt[i] = entry & ~4;
            } else {
                pt[i] = 0;
            }
        }
        pd[t] = (uint32_t)pt | 7;  // present, writable, user (PTEs gate per-page)
    }

    return pd;
}

void* clone_page_directory(void) {
    return alloc_page_directory();
}

void map_page_dir(uint32_t* pd, void* phys, void* virt, uint32_t flags) {
    if (!pd) pd = kernel_page_directory;
    uint32_t pd_idx = (uint32_t)virt >> 22;
    uint32_t pt_idx = ((uint32_t)virt >> 12) & 0x3FF;

    if (!(pd[pd_idx] & 1)) {
        uint32_t* pt = (uint32_t*)alloc_page();
        if (!pt) return;
        memset_asm(pt, 0, PAGE_SIZE);
        uint32_t pde_flags = (pd == kernel_page_directory) ? 3 : 7;
        pd[pd_idx] = (uint32_t)pt | pde_flags;
    }
    uint32_t* pt = (uint32_t*)(pd[pd_idx] & ~0xFFF);
    pt[pt_idx] = ((uint32_t)phys & ~0xFFF) | (flags & 0xFFF) | 1;

    if (pd == current_page_directory || pd == kernel_page_directory)
        __asm__ volatile("invlpg (%0)" :: "r"(virt));
}
