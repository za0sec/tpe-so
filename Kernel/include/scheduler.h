#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <memManager.h>
#include <interrupts.h>
#include <pcb_queue.h>
#include <stdint.h>

#define STACK_SIZE 4096
#define DEFAULT_QUANTUM 5
#define CPU_BOUND_QUANTUM 3
#define IO_BOUND_QUANTUM 7
#define TOTAL_QUEUES 5
typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

// Assembly functions
void asm_cli();
void asm_sti();
void asm_halt();

// Scheduler functions
void init_scheduler();
uint64_t schedule(void *running_process_rsp);
pcb_t get_next_process();
void initProcessWrapper(program_t program, uint64_t argc, char **argv);

// Process management
uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[], uint64_t *fd_ids, uint64_t fd_count);
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t *fd_ids, uint64_t fd_count);
pcb_t create_process_halt();
uint64_t kill_process(uint64_t pid);
uint64_t block_process();
uint64_t block_current_process_to_queue(q_adt blocked_queue);
uint64_t block_process_to_queue(uint64_t pid, q_adt dest);
uint64_t unblock_process_from_queue(q_adt src);
uint64_t unblock_process(uint64_t pid);
uint64_t get_pid();
pcb_t get_current_process();
int remove_file_descriptor_current_process(open_file_t *fd);
void wait_pid(uint64_t pid);
void list_processes(char *buf);
void list_processes();
void yield();

// Queue management
uint8_t add_priority_queue(pcb_t process);
pcb_t find_dequeue_priority(uint64_t pid);

// Stack management
void* fill_stack(uintptr_t sp, void (*initProcessWrapper)(program_t, uint64_t, char**), program_t program, uint64_t argc, char** argv);

#endif
