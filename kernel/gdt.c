#include "kernel.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct gdt_entry gdt[6];
static struct gdt_ptr gp;

// TSS structure
static tss_entry_t tss;

extern void gdt_flush(uint32_t);

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void tss_set_stack(uint32_t esp0) {
    tss.esp0 = esp0;
    tss.ss0 = KERNEL_DS;
}

void load_tss(void) {
    __asm__ volatile("ltr %%ax" : : "a"(TSS_SEL));
}

void init_gdt(void) {
    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                            // null
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);            // kernel code
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);            // kernel data
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);            // user code
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);            // user data

    // Initialize TSS
    memset_asm(&tss, 0, sizeof(tss));
    tss.ss0 = KERNEL_DS;
    tss.iomap_base = sizeof(tss);  // deny all I/O ports from ring 3

    uint32_t tss_base = (uint32_t)&tss;
    uint32_t tss_limit = sizeof(tss) - 1;
    gdt_set_gate(5, tss_base, tss_limit, 0x89, 0x00);      // TSS (present, 32-bit available)

    gdt_flush((uint32_t)&gp);
    load_tss();
}
