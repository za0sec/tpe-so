// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <videoDriver.h>
#include <keyboard.h>
#include <lib.h>
#include <time.h>
#include <sound.h>
#include <memManager.h>
#include <stdint.h>
#include <scheduler.h>
#include <semaphore.h>
#include <pipe.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern uint8_t hasregisterInfo;
extern const uint64_t registerInfo[17];
extern int _hlt();

extern Color WHITE;
extern Color BLACK;

/* Funciones del sistema */

// Llena buff con el carácter leído del teclado
static uint64_t sys_read(uint64_t fd) {
    if (fd != STDIN) {
        return -1;
    }

    return read_keyboard();
}

static uint64_t sys_write(uint64_t fd, char buffer) {
    if (fd != STDOUT) {
        return -1;
    }

    vDriver_print(buffer, WHITE, BLACK);
    return 1;
}

static uint64_t sys_clear() {
    vDriver_clear();
    return 1;
}

static uint64_t sys_getHours() {
    return getHours();
}

static uint64_t sys_getMinutes() {
    return getMinutes();
}

static uint64_t sys_getSeconds() {
    return getSeconds();
}

static uint64_t sys_getScrHeight() {
    return vDriver_getHeight();
}

static uint64_t sys_getScrWidth() {
    return vDriver_getWidth();
}

static void sys_drawRectangle(int x, int y, int x2, int y2, Color color) {
    vDriver_drawRectangle(x, y, x2, y2, color);
}

static void sys_wait(int ms) {
    timer_wait(ms);
}

static uint64_t sys_registerInfo(uint64_t registers[17]) {
    if (hasregisterInfo) {
        for (uint8_t i = 0; i < 17; i++) {
            registers[i] = registerInfo[i];
        }
    }
    return hasregisterInfo;
}

static uint64_t sys_printmem(uint64_t *address) {
    if ((uint64_t)address > (0x20000000 - 32)) {
        return -1;
    }

    uint8_t *aux = (uint8_t *)address;
    vDriver_prints("\n", WHITE, BLACK);
    for (int i = 0; i < 32; i++) {
        vDriver_printHex((uint64_t)aux, WHITE, BLACK);
        vDriver_prints(" = ", WHITE, BLACK);
        vDriver_printHex(*aux, WHITE, BLACK);
        vDriver_newline();
        aux++;
    }

    return 0;
}

static uint64_t sys_pixelPlus() {
    plusScale();
    return 1;
}

static uint64_t sys_pixelMinus() {
    minusScale();
    sys_clear();
    return 1;
}

static uint64_t sys_playSpeaker(uint32_t frequency, uint64_t duration) {
    beep(frequency, duration);
    return 1;
}

static uint64_t sys_stopSpeaker() {
    stopSpeaker();
    return 1;
}

static uint64_t sys_drawCursor() {
    vDriver_drawCursor();
    return 1;
}

static uint64_t sys_writeColor(uint64_t fd, char buffer, Color color) {
    if (fd != STDOUT) {
        return -1;
    }

    vDriver_print(buffer, color, BLACK);
    return 1;
}

static void *sys_mem_alloc(uint64_t size) {
    return mem_alloc(size);
}

static void sys_mem_free(void *ptr) {
    mem_free(ptr);
}

static void sys_mem_init(void *ptr, int s) {
    mem_init(ptr, s);
}

/* Funciones del scheduler */

// Si le pasas fd_count < 2, se le asignan los fd de stdin y stdout!
static uint64_t sched_create_process(int priority, program_t program, uint64_t argc, char *argv[]) {
    return userspace_create_process(priority, program, argc, argv);
}

static uint64_t sched_kill_process(uint64_t pid) {
    return kill_process(pid);
}

static uint64_t sched_getPID() {
    return get_pid();
}

static char * sched_list_processes() {
    return list_processes();
}

static void sched_block_process(uint64_t pid) {
    block_process_pid(pid);
}

static void sched_unblock_process(uint64_t pid) {
    unblock_process(pid);
}

static void sched_yield() {
    yield();
}

/* Funciones de semáforos */

static int sys_sem_open(char *sem_name, int init_value) {
    return sem_open(sem_name, init_value);
}

static void sys_sem_close(char *sem) {
    sem_close(sem);
}

static void sys_sem_wait(char *sem) {
    sem_wait(sem);
}

static void sys_sem_post(char *sem) {
    sem_post(sem);
}

static void sys_wait_pid(uint64_t pid) {
    wait_pid(pid);
}

static char sys_read_fd(uint64_t process_fd_index) {
    return fd_read_current_process(process_fd_index);
}

static char sys_write_fd(uint64_t process_fd_index, char data){
    return fd_write_current_process(process_fd_index, data);
}

static uint64_t sys_open_fd(uint64_t fd_id){
    return fd_open_current_process(fd_id);
}

static uint64_t sys_close_fd(uint64_t fd_index){
    return fd_close_current_process(fd_index);
}

static uint64_t sched_create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[]){
    return userspace_create_process_foreground(priority, program, argc, argv);
}

static void sched_create_process_set_fd(int *fd_ids, uint64_t fd_count){
    return userspace_set_fd(fd_ids, fd_count);
}

static uint64_t sys_pipe_create(){
    return pipe_create();
}

static uint64_t sys_set_priority(uint64_t pid, uint8_t priority){
    return set_priority(pid, priority);
}

static char* sys_mem_state(){
    return mem_state();
}

/* Arreglo de punteros a funciones (syscalls) */

static uint64_t (*syscalls[])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {
    /* RAX: número de syscall */
    (void *)sys_read,      // 0
    (void *)sys_write,              // 1
    (void *)sys_clear,              // 2
    (void *)sys_getHours,           // 3
    (void *)sys_getMinutes,         // 4
    (void *)sys_getSeconds,         // 5
    (void *)sys_getScrHeight,       // 6
    (void *)sys_getScrWidth,        // 7
    (void *)sys_drawRectangle,      // 8
    (void *)sys_wait,               // 9
    (void *)sys_registerInfo,       // 10
    (void *)sys_printmem,           // 11
    (void *)sys_pixelPlus,          // 12
    (void *)sys_pixelMinus,         // 13
    (void *)sys_playSpeaker,        // 14
    (void *)sys_stopSpeaker,        // 15
    (void *)sys_drawCursor,         // 16
    (void *)sys_writeColor,         // 17
    (void *)sys_mem_alloc,          // 18
    (void *)sys_mem_free,           // 19
    (void *)sys_mem_init,           // 20
    (void *)sched_create_process,   // 21
    (void *)sched_kill_process,     // 22
    (void *)sched_getPID,           // 23
    (void *)sched_list_processes,   // 24
    (void *)sched_block_process,    // 25
    (void *)sched_unblock_process,  // 26
    (void *)sched_yield,            // 27
    (void *)sys_sem_open,           // 28
    (void *)sys_sem_close,          // 29
    (void *)sys_sem_wait,           // 30
    (void *)sys_sem_post,           // 31
    (void *)sys_wait_pid,           // 32
    (void *)sys_read_fd,            // 33
    (void *)sys_write_fd,           // 34
    (void *)sys_open_fd,            // 35
    (void *)sys_close_fd,           // 36
    (void *)sched_create_process_foreground, // 37
    (void *)sched_create_process_set_fd,     // 38
    (void *)sys_pipe_create,        // 39
    (void *)sys_set_priority,       // 40
    (void *)sys_mem_state               // 41
};

uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax) {
    if (syscalls[rax] != 0){
        return syscalls[rax](rdi, rsi, rdx, r10, r8);
    }

    return 0;
}