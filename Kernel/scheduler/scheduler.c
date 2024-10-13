#include <scheduler.h>
#include <memManager.h>
#include <pcb_queue.h>

uint64_t currentPID = 0;
pcb_t current_process;
q_adt process_queue = NULL;
q_adt blocked_queue = NULL;

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[]){
    uint64_t sp = mem_alloc(STACK_SIZE);
    fill_stack(sp, &initProcessWrapper, program, argc, argv);
    pcb_t new_process = {
                        currentPID++,       //pid
                        sp + STACK_SIZE,    //rsp
                        DEFAULT_QUANTUM,    //assigned_quantum
                        0,                  //used_quantum
                        READY               //state
                        };
    add(process_queue, new_process);
    return new_process.pid;
}

//                            RDI             RSI            RDX      
void initProcessWrapper(program_t program, uint64_t argc, char *argv[]){
    uint64_t ret_value = program(argc, argv);
    if(ret_value != 0){
        // TODO: handle error
    }
    current_process.state = TERMINATED;
    //TODO: Aca deberia haber un exit(1) no??
}

void halt(){
    while(1){
        asm_halt();
    }
}

void create_process_halt(){
    create_process(0, &halt, 0, NULL);
}

void init_scheduler(){
    process_queue = new_q();    
    blocked_queue = new_q();
}

uint64_t schedule(uint64_t running_process_rsp){
    asm_cli();

    current_process.rsp = running_process_rsp;
    if(current_process.state == RUNNING){
        current_process.used_quantum++;
        if(current_process.used_quantum < current_process.assigned_quantum){
            return current_process.rsp;
        } else {
            current_process.state = READY;
            current_process.used_quantum = 0;
            current_process.assigned_quantum--;

            if(current_process.assigned_quantum <= 0){
                current_process.assigned_quantum = CPU_BOUND_QUANTUM;
            }

            add(process_queue, current_process);
        }
    } else if (current_process.state == BLOCKED){
        if(++current_process.used_quantum < current_process.assigned_quantum && current_process.assigned_quantum < IO_BOUND_QUANTUM){
            current_process.assigned_quantum++;
        }
        add(blocked_queue, current_process);        
    } else {
        //Process is TERMINATED
        mem_free(current_process.rsp);
    } 
    
    if(!has_next(process_queue)){
        create_process_halt();
    }
    current_process = dequeue(process_queue);
    current_process.state = RUNNING;

    asm_sti();
    return current_process.rsp;
}