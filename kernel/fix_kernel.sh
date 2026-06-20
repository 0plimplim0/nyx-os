#!/bin/bash
# Script para generar los archivos faltantes y corregir el kernel

# 1. Crear stubs.asm correcto
cat << 'EOF' > stubs.asm
; stubs.asm - Funciones de bajo nivel para NyxOS

global gdt_flush, idt_flush
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9
global isr10, isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19
global isr20, isr21, isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29
global isr30, isr31
global irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9
global irq10, irq11, irq12, irq13, irq14, irq15
global outb, inb, outw, inw, outl, inl, io_wait
global enable_interrupts, disable_interrupts
global read_cr0, write_cr0, read_cr3, write_cr3, flush_tlb, invlpg
global memcpy_asm, memset_asm

; Rutinas de interrupción (ISR)
isr0:  iret
isr1:  iret
isr2:  iret
isr3:  iret
isr4:  iret
isr5:  iret
isr6:  iret
isr7:  iret
isr8:  iret
isr9:  iret
isr10: iret
isr11: iret
isr12: iret
isr13: iret
isr14: iret
isr15: iret
isr16: iret
isr17: iret
isr18: iret
isr19: iret
isr20: iret
isr21: iret
isr22: iret
isr23: iret
isr24: iret
isr25: iret
isr26: iret
isr27: iret
isr28: iret
isr29: iret
isr30: iret
isr31: iret

; Rutinas de IRQ
irq0:  iret
irq1:  iret
irq2:  iret
irq3:  iret
irq4:  iret
irq5:  iret
irq6:  iret
irq7:  iret
irq8:  iret
irq9:  iret
irq10: iret
irq11: iret
irq12: iret
irq13: iret
irq14: iret
irq15: iret

; I/O ports
outb:
    mov dx, [esp+4]
    mov al, [esp+8]
    out dx, al
    ret

inb:
    mov dx, [esp+4]
    in al, dx
    ret

outw:
    mov dx, [esp+4]
    mov ax, [esp+8]
    out dx, ax
    ret

inw:
    mov dx, [esp+4]
    in ax, dx
    ret

outl:
    mov dx, [esp+4]
    mov eax, [esp+8]
    out dx, eax
    ret

inl:
    mov dx, [esp+4]
    in eax, dx
    ret

io_wait:
    out 0x80, al
    ret

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

read_cr0:
    mov eax, cr0
    ret

write_cr0:
    mov eax, [esp+4]
    mov cr0, eax
    ret

read_cr3:
    mov eax, cr3
    ret

write_cr3:
    mov eax, [esp+4]
    mov cr3, eax
    ret

flush_tlb:
    mov eax, cr3
    mov cr3, eax
    ret

invlpg:
    mov eax, [esp+4]
    invlpg [eax]
    ret

gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    ret

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

memcpy_asm:
    push edi
    push esi
    push ecx
    mov edi, [esp+16]
    mov esi, [esp+20]
    mov ecx, [esp+24]
    rep movsb
    pop ecx
    pop esi
    pop edi
    ret

memset_asm:
    push edi
    push ecx
    mov edi, [esp+12]
    mov al, [esp+16]
    mov ecx, [esp+20]
    rep stosb
    pop ecx
    pop edi
    ret
EOF

# 2. Crear umoddi3.c
cat << 'EOF' > umoddi3.c
unsigned long long __umoddi3(unsigned long long a, unsigned long long b) {
    while (a >= b) a -= b;
    return a;
}
EOF

# 3. Asegurar que backdoor.c tenga la función correcta
if ! grep -q "module_backdoor_init" ../modules/backdoor.c; then
    cat << 'EOF' > ../modules/backdoor.c
#include "../kernel/kernel.h"
void* module_backdoor_init(void) {
    printf("  [BACKDOOR] Module loaded\n");
    return NULL;
}
EOF
fi

# 4. Actualizar Makefile: asegurar que stubs.o y umoddi3.o estén en los objetos
if ! grep -q "stubs.o" makefile; then
    # Añadir stubs.o y umoddi3.o a la línea de objetos (asumiendo que KERNEL_OBJS ya existe)
    sed -i 's/serial.o/serial.o stubs.o umoddi3.o/' makefile
fi

echo "Archivos generados. Ahora ejecuta 'make clean && make'."