#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <memManager.h>
#include <interrupts.h>
#include <pcb_queue.h>
#include <stdint.h>

#ifndef STACK_SIZE_VALUE
#define STACK_SIZE_VALUE 4096
#endif

#define STACK_SIZE STACK_SIZE_VALUE
#define TOTAL_QUEUES 5
#define HIGHEST_QUEUE 3
#define AGING_THRESHOLD 100
#define ASSIGN_QUANTUM(priority) ((priority + 1) * 2)
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
void userspace_set_fd(int *fd_ids, int fd_count);
uint64_t userspace_create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[]);
uint64_t userspace_create_process(int priority, program_t program, uint64_t argc, char *argv[]);
uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[], uint64_t *fd_ids, uint64_t fd_count);
uint64_t create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[MAX_FD], uint64_t fd_count);
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t *fd_ids, uint64_t fd_count, int p_pid);
int find_process_in_queue(q_adt q, uint64_t pid);
int find_process_in_priority_queues(uint64_t pid);
pcb_t get_process_by_pid(uint64_t pid);
uint64_t set_priority(uint64_t pid, uint8_t priority);
pcb_t create_process_halt();
uint64_t kill_process(uint64_t pid);
uint64_t kill_process_foreground();
uint64_t kill_process_terminal(char * pid);
uint64_t block_process();
void block_process_pid(uint64_t pid);
void send_EOF_foreground();
uint64_t block_current_process_to_queue(q_adt blocked_queue);
uint64_t block_process_to_queue(uint64_t pid, q_adt dest);
uint64_t unblock_process_from_queue(q_adt src);
uint64_t unblock_process(uint64_t pid);
uint64_t get_pid();
pcb_t get_current_process();
int remove_file_descriptor_current_process(open_file_t *fd);
void wait_pid(uint64_t pid);
char * list_processes();
void yield();
void terminate_process(pcb_t process);

// Queue management
uint8_t add_priority_queue(pcb_t process);
pcb_t find_dequeue_priority(uint64_t pid);

// Stack management
void* fill_stack(uintptr_t sp, void (*initProcessWrapper)(program_t, uint64_t, char**), program_t program, uint64_t argc, char** argv);

#endif
