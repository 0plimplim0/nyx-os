; Minimal ELF test program for NyxOS
; Uses int 0x80 syscalls:
;   syscall 2: print string (ebx = string pointer)
;   syscall 0: exit (ebx = exit code)

BITS 32
GLOBAL _start

SECTION .text

_start:
    ; Print "Hello from ELF!\n"
    mov eax, 2          ; syscall: print string
    mov ebx, hello_msg  ; arg1: string pointer
    int 0x80

    ; Print "Second line!\n"
    mov eax, 2
    mov ebx, second_msg
    int 0x80

    ; Exit
    mov eax, 0          ; syscall: exit
    mov ebx, 42         ; exit code
    int 0x80

    ; Fallback: infinite loop
.loop:
    hlt
    jmp .loop

SECTION .data

hello_msg:  db "Hello from ELF on NyxOS!", 10, 0
second_msg: db "This is ring 3 user mode!", 10, 0
