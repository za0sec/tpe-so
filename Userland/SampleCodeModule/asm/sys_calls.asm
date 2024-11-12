GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_clear
GLOBAL sys_getMinutes
GLOBAL sys_getHours
GLOBAL sys_getSeconds
GLOBAL sys_scrHeight
GLOBAL sys_scrWidth
GLOBAL sys_drawRectangle
GLOBAL sys_wait
GLOBAL sys_registerInfo
GLOBAL sys_drawCursor
GLOBAL sys_printmem
GLOBAL sys_pixelMinus
GLOBAL sys_pixelPlus
GLOBAL sys_playSpeaker
GLOBAL sys_writeColor
GLOBAL sys_stopSpeaker
GLOBAL sys_mem_alloc
GLOBAL sys_mem_free
GLOBAL sys_mem_init
GLOBAL sys_create_process
GLOBAL sys_create_process_set_fd
GLOBAL sys_create_process_foreground
GLOBAL sys_kill
GLOBAL sys_getPID
GLOBAL sys_list_processes
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_yield
GLOBAL sys_sem_open
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL sys_sem_close
GLOBAL sys_wait_pid
GLOBAL sys_write_fd
GLOBAL sys_read_fd
GLOBAL sys_open_fd
GLOBAL sys_close_fd
GLOBAL sys_pipe_create
GLOBAL sys_nice
GLOBAL sys_kill_terminal

; Pasaje de parametros en C:
; %rdi %rsi %rdx %rcx %r8 %r9

; Pasaje de parametros para sys_calls
; %rdi %rsi %rdx %r10 %r8 %r9

; MOVER RCX a R10

; %rcx y %rc11 son destruidos si las llamo desde aca

; %rax el numero de la syscall

sys_read:
    mov rax, 0x00
    int 80h
    ret

sys_write:
    mov rax, 0x01
    int 80h
    ret

sys_clear:
    mov rax, 0x02
    int 80h
    ret

sys_getHours:
    mov rax, 0x03
    int 80h
    ret

sys_getMinutes:
    mov rax, 0x04
    int 80h
    ret

sys_getSeconds:
    mov rax, 0x05
    int 80h
    ret

sys_scrHeight:
    mov rax, 0x06
    int 80h
    ret

sys_scrWidth:
    mov rax, 0x07
    int 80h
    ret

sys_drawRectangle:
    mov rax, 0x08
    mov r10, rcx        ;4to parametro de syscall es R10
    int 80h
    ret

sys_wait:
    mov rax, 0x09
    int 80h
    ret

sys_registerInfo:
    mov rax, 0x0A
    int 80h
    ret

sys_printmem: 
    mov rax, 0x0B
    int 80h
    ret

sys_pixelPlus: 
    mov rax, 0x0C
    int 80h
    ret

sys_pixelMinus: 
    mov rax, 0x0D
    int 80h
    ret

sys_playSpeaker: 
    mov rax, 0x0E
    int 80h
    ret

sys_stopSpeaker: 
    mov rax, 0x0F
    int 80h
    ret

sys_drawCursor:
    mov rax, 0x10
    int 80h
    ret

sys_writeColor:
    mov rax, 0x11
    int 80h
    ret

sys_mem_alloc:
    mov rax, 0x12
    int 80h
    ret

sys_mem_free:
    mov rax, 0x13
    int 80h
    ret

sys_mem_init:
    mov rax, 0x14
    int 80h
    ret

sys_create_process:
    mov rax, 0x15
    mov r10, rcx        ;4to parametro de syscall es R10
    int 80h
    ret

sys_kill:
    mov rax, 0x16
    int 80h
    ret

sys_getPID:
    mov rax, 0x17
    int 80h
    ret

sys_list_processes:
    mov rax, 0x18
    int 80h
    ret

sys_block:
    mov rax, 0x19
    int 80h
    ret

sys_unblock:
    mov rax, 0x1A
    int 80h
    ret

sys_yield:
    mov rax, 0x1B
    int 80h
    ret

sys_sem_open:
    mov rax, 0x1C
    int 80h
    ret

sys_sem_close:
    mov rax, 0x1D
    int 80h
    ret

sys_sem_wait:
    mov rax, 0x1E
    int 80h
    ret

sys_sem_post:
    mov rax, 0x1F
    int 80h
    ret

sys_wait_pid:
    mov rax, 0x20
    int 80h
    ret

sys_read_fd:
    mov rax, 0x21
    int 80h
    ret

sys_write_fd:
    mov rax, 0x22
    int 80h
    ret

sys_open_fd:
    mov rax, 0x23
    int 80h
    ret

sys_close_fd:
    mov rax, 0x24
    int 80h
    ret

sys_create_process_foreground:
    mov rax, 0x25
    mov r10, rcx        ;4to parametro de syscall es R10
    int 80h
    ret

sys_create_process_set_fd:
    mov rax, 0x26
    int 80h
    ret

sys_pipe_create:
    mov rax, 0x27
    int 80h
    ret

sys_nice:
    mov rax, 0x28
    int 80h
    ret