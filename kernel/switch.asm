; switch.asm - x86_64 context switch for NyxOS
default rel
BITS 64

global switch_context
global create_task_stack
global ku_setjmp
global ku_longjmp

; ku_setjmp(uint64_t* buf)  -> returns 0 the first time, or `val` when resumed
; via ku_longjmp. buf layout: rbx,rbp,r12,r13,r14,r15,rsp(post-ret),return-addr.
; Used to return control to switch_to_user_process's caller when a user process
; calls exit() from within the (separate) syscall stack.
ku_setjmp:
    mov [rdi + 0],  rbx
    mov [rdi + 8],  rbp
    mov [rdi + 16], r12
    mov [rdi + 24], r13
    mov [rdi + 32], r14
    mov [rdi + 40], r15
    lea rax, [rsp + 8]          ; RSP as it will be after our RET
    mov [rdi + 48], rax
    mov rax, [rsp]             ; return address
    mov [rdi + 56], rax
    xor eax, eax               ; first return: 0
    ret

; ku_longjmp(uint64_t* buf, uint64_t val)  -- never returns to caller
ku_longjmp:
    mov rbx, [rdi + 0]
    mov rbp, [rdi + 8]
    mov r12, [rdi + 16]
    mov r13, [rdi + 24]
    mov r14, [rdi + 32]
    mov r15, [rdi + 40]
    mov rsp, [rdi + 48]        ; restore stack to setjmp's post-RET point
    mov rax, rsi              ; return value
    test rax, rax
    jnz .nz
    inc rax                   ; longjmp(,0) must appear as 1
.nz:
    mov rcx, [rdi + 56]
    jmp rcx                   ; resume as if ku_setjmp returned `val`

; switch_context(uint64_t* old_rsp_ptr, uint64_t new_rsp)
; RDI = old_rsp_ptr, RSI = new_rsp (x86_64 ABI)
switch_context:
    push rbx
    push rcx
    push rdx
    push rsi           ; saved RSI = new_rsp
    push rdi           ; saved RDI = old_rsp_ptr
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; [RSP+0]=r15, [RSP+8]=r14, ..., [RSP+96]=rdi, [RSP+104]=rsi
    ; Read saved args from stack
    mov rax, [rsp + 96]        ; old_rsp_ptr
    mov [rax], rsp             ; *old_rsp_ptr = current RSP
    mov rsp, [rsp + 104]       ; RSP = new_rsp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; create_task_stack(uint64_t stack_top, uint64_t entry_point)
; RDI = stack_top, RSI = entry_point
create_task_stack:
    mov rax, rdi                ; stack_top
    mov rcx, rsi                ; entry_point
    mov [rax - 8], rcx          ; return address = entry_point
    mov qword [rax - 16], 0     ; r15
    mov qword [rax - 24], 0     ; r14
    mov qword [rax - 32], 0     ; r13
    mov qword [rax - 40], 0     ; r12
    mov qword [rax - 48], 0     ; r11
    mov qword [rax - 56], 0     ; r10
    mov qword [rax - 64], 0     ; r9
    mov qword [rax - 72], 0     ; r8
    mov qword [rax - 80], 0     ; rbp
    mov qword [rax - 88], 0     ; rdi
    mov qword [rax - 96], 0     ; rsi
    mov qword [rax - 104], 0    ; rdx
    mov qword [rax - 112], 0    ; rcx
    mov qword [rax - 120], 0    ; rbx
    mov qword [rax - 128], 0    ; rax
    lea rax, [rax - 128]
    ret
