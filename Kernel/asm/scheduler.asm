global tick_handler
global asm_halt
global asm_cli
global asm_sti
global fill_stack
extern schedule

section .text

%macro pushState 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    pushfq        ; Guarda el registro de banderas (flags)
%endmacro

%macro popState 0
    popfq         ; Restaura el registro de banderas (flags)
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
%endmacro

tick_handler:
    pushState

    mov rdi, rsp
    call schedule
    mov rsp, rax

    ; Enviar EOI
    mov al, 0x20
    out 0x20, al

    popState
    pop rax
    iretq

fill_stack:
    push rbp
    mov rbp, rsp
    
    mov rsp, rdi

    push 0x0
    push rsi        ;   SP
    push 0x202      ;   RFLAGS
    push 0x8        ;   CS
                    ;   > Preparo argumentos para initProcessWrapper 
    mov rdi, rdx    ;   program
    mov rsi, rcx    ;   argc
    mov rdx, r8     ;   argv
    push rsi        ;   RIP = initProcessWrapper
    pushState       ;   Cargo algun estado de registros

    mov rsp, rbp
    pop rbp

    ret

asm_halt:
    hlt
    ret

asm_cli:
    cli
    ret

asm_sti:
    sti
    ret