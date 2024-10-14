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
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
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
    ;pop rax ; debug a ver si esta el RIP de initProcessWrapper
    iretq

fill_stack:
    push rbp
    mov rbp, rsp
    
    mov rsp, rdi

    push 0x0
    push rdi        ;   SP
    push 0x202      ;   RFLAGS
    push 0x8        ;   CS
                    ;   > Preparo argumentos para initProcessWrapper 
    push rsi        ;   RIP = initProcessWrapper
    mov rdi, rdx    ;   program (cargo argumento)
    mov rsi, rcx    ;   argc    (cargo argumento)
    mov rdx, r8     ;   argv    (cargo argumento)
    pushState       ;   Cargo el estado de registros (que incluye los args para initProcessWrapper)
    mov rax, rsp    ;   RETURN NEW RSP
    
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