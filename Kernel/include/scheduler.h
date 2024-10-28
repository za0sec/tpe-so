#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <memManager.h>
#include <interrupts.h>
#include <pcb_queue.h>

typedef struct {
    lock_t sem_lock;
    char name[32];    
    int value;           // Valor del semáforo
    q_adt blocked_queue; // Cola de procesos bloqueados
} sem_t;

#define STACK_SIZE 4096
#define DEFAULT_QUANTUM 5
#define CPU_BOUND_QUANTUM 3
#define IO_BOUND_QUANTUM 7
#define MAX_SEMAPHORES CHUNK_SIZE / sizeof(sem_t)
#define SEMAPHORE_NAME_SIZE 32

#include <stdint.h>
#include <pcb_queue.h>

typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

void asm_cli();
void asm_sti();
void asm_halt();

void tick_handler();
uint64_t schedule(void *running_process_rsp);
//                RDI             RSI                                                   RDX                  RCX                 R8              R9
void* fill_stack(uintptr_t sp, void (*initProcessWrapper)(program_t, uint64_t, char**), program_t program, uint64_t argc, char** argv);
void initProcessWrapper(program_t program, uint64_t argc, char **argv);
void init_scheduler();
q_adt init_schedule();

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[]);
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[]);
uint64_t kill_process(uint64_t pid);
void list_processes(char *buf);
uint64_t block_process(uint64_t);
uint64_t block_process_to_queue(uint64_t pid, q_adt blocked_queue);
uint64_t unblock_process_from_queue(q_adt blocked_queue);
uint64_t unblock_process(uint64_t pid);
uint64_t get_pid();
void yield();

#endif
