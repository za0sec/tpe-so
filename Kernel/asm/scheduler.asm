global tick_handler
extern schedule
section .text

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
