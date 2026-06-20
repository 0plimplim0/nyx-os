BITS 32
global _start
global memcpy_asm, memset_asm
extern kernel_main

MAGIC    equ 0x1BADB002
FLAGS    equ 0x00000003
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

section .bss
align 16
stack_bottom:
    resb 131072
stack_top:

section .text
_start:
    mov esp, stack_top
    push ebx
    push eax
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    xor ebp, ebp
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang

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