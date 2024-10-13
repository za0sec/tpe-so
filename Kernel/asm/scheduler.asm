global tick_handler
global asm_halt
extern schedule
section .text


// Codigo de la presentacion de la clase practica
tick_handler:
    pushState

    mov rdi, rsp
    call schedule
    mov rsp, rax

    ; Enviar EOI
    mov al, 0x20
    out 0x20, al

    popState

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