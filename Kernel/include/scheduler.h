#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define STACK_SIZE 128
#define DEFAULT_QUANTUM 5
#define CPU_BOUND_QUANTUM 3
#define IO_BOUND_QUANTUM 7

#include <stdint.h>
#include <pcb_queue.h>

typedef int64_t (*program_t)(uint64_t argc, char *argv[]);

void asm_cli();
void asm_sti();
void asm_halt();

void tick_handler();
uint64_t schedule(uint64_t running_process_rsp);
void fill_stack(uint64_t sp, uint64_t * initProcessWrapper, program_t program, uint64_t argc, char* argv[]);
void initProcessWrapper(program_t program, uint64_t argc, char *argv[]);
void init_scheduler();
q_adt init_schedule();

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[]);
void kill_process(uint64_t pid);
void list_processes(char *buf);
void block_process(uint64_t pid);
void unblock_process(uint64_t pid);
void get_PID();
void yield();

#endif
