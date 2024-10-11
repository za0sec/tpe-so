#include <scheduler.h>
#include <memManager.h>
#include <queue.h>
#define MAX_PROCESSES 10

struct PCB processes[MAX_PROCESSES];
int current_process = -1;\
uint64_t currentPID = 0;

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[]){
    uint64_t sp = mem_alloc(STACK_SIZE);
    fill_stack(sp, &initProcessWrapper, program, argc, argv);
    pcb_t new_process = {
                        currentPID++,
                        sp,
                        READY
                        };
    //Agregar PCB a la cola FIFO
}

//                            RDI             RSI            RDX      
void initProcessWrapper(program_t program, uint64_t argc, char *argv[]){
    uint64_t ret_value = program(argc, argv);
    if(ret_value != 0){
        // TODO: handle error
    }
    exit(1);
}

uint64_t schedule(uint64_t running_process_rsp){

}