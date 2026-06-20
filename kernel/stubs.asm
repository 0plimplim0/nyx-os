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

; Rutinas de interrupción (ISR) – permanecen simples de momento
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

; Rutinas de IRQ con EOI y llamada a irq_handler
; Macro para generar stub IRQ
%macro IRQ_STUB 2
  irq%1:
    push 0          ; código de error falso
    push %2        ; número de interrupción
    jmp irq_common
%endmacro

IRQ_STUB 0,  32
IRQ_STUB 1,  33
IRQ_STUB 2,  34
IRQ_STUB 3,  35
IRQ_STUB 4,  36
IRQ_STUB 5,  37
IRQ_STUB 6,  38
IRQ_STUB 7,  39
IRQ_STUB 8,  40
IRQ_STUB 9,  41
IRQ_STUB 10, 42
IRQ_STUB 11, 43
IRQ_STUB 12, 44
IRQ_STUB 13, 45
IRQ_STUB 14, 46
IRQ_STUB 15, 47

irq_common:
    ; Guardar todos los registros de propósito general
    pusha

    ; Llamar al manejador C (void irq_handler(uint32_t irq_no))
    ; El número de interrupción está en [esp+32] (después de pusha y los dos pushes)
    mov eax, [esp+36]      ; Obtener el número de interrupción
    push eax               ; Pasarlo como argumento
    extern irq_handler
    call irq_handler
    add esp, 4             ; Limpiar argumento

    ; Enviar EOI al PIC correspondiente
    mov al, 0x20
    cmp byte [esp+36], 40  ; ¿Es del PIC esclavo? (IRQ8-15 -> INT 40-47)
    jb .master_only
    out 0xA0, al           ; EOI al esclavo
.master_only:
    out 0x20, al           ; EOI al maestro

    popa
    add esp, 8             ; Eliminar código de error y número de interrupción
    iret

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