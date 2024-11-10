#include <scheduler.h>
#include <memManager.h>
#include <pcb_queue.h>
#include <utils.h>
#include <file_descriptor.h>
#include <open_file_t.h>

uint64_t currentPID = 0;
pcb_t current_process;
pcb_t halt_process;

q_adt p0 = NULL;
q_adt p1 = NULL;
q_adt p2 = NULL;
q_adt p3 = NULL;

q_adt all_blocked_queue = NULL;                 // Procesos bloqueados mediante syscall
q_adt blocked_read_queue = NULL;

q_adt blocked_semaphore_queue = NULL;
int current_semaphore = 0;
uint8_t mutex_lock = 1;

uint64_t schedule(void *running_process_rsp){
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
                //Esto deberia ser decrementar LA PRIORIDAD
                current_process.assigned_quantum = CPU_BOUND_QUANTUM;
            }

            add_priority_queue(current_process);
        }
    } else if (current_process.state == BLOCKED){
        if(++current_process.used_quantum < current_process.assigned_quantum && current_process.assigned_quantum < IO_BOUND_QUANTUM){
            //Esto deberia ser aumentar LA PRIORIDAD
            current_process.assigned_quantum++;
        }
        add(all_blocked_queue, current_process);
    } else if (current_process.state == READY){
        add_priority_queue(current_process);
    } else if (current_process.state == TERMINATED){
        for(int i = 0; i < MAX_FD; i++){
            if(current_process.fd_table[i] != NULL){
                fd_manager_close(current_process.fd_table[i]->id);
            }
        }
        mem_free(current_process.rsp);
    } // else: VENGO DE UN HALT!

    pcb_t next_process = get_next_process();
    
    if(next_process.state != READY){
        if(current_process.state == HALT){
            return current_process.rsp;
        }
        current_process = create_process_halt();
    } else {
        current_process = next_process;
        current_process.state = RUNNING;
    }

    return current_process.rsp;
}

pcb_t get_next_process(){
    pcb_t next_process;
    if(has_next(p0)){
        next_process = dequeue(p0);
    } else if(has_next(p1)){
        next_process = dequeue(p1);
    } else if(has_next(p2)){
        next_process = dequeue(p2);
    } else if(has_next(p3)){
        next_process = dequeue(p3);
    } else {
        return (pcb_t){-1, 0, 0, 0, 0, TERMINATED, NULL};
    }
    return next_process;
}

pcb_t get_current_process(){
    return current_process;
}

// Retorna -1 por error
uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[MAX_FD], uint64_t fd_count){
    return create_process_state(priority, program, READY, argc, argv, fd_ids, fd_count);
}

// Retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t fd_ids[MAX_FD], uint64_t fd_count){    
    void *base_pointer = mem_alloc(STACK_SIZE);
    
    if(base_pointer == NULL){
        return -1;
    }

    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, program, argc, argv);
    open_file_t **fd_table = fd_manager_open_fd_table(fd_ids, fd_count);

    pcb_t new_process = {
                        currentPID++,       //pid
                        stack_pointer,      //rsp
                        priority,           //priority
                        DEFAULT_QUANTUM,    //assigned_quantum
                        0,                  //used_quantum
                        state,              //state
                        fd_table
                        };

    add_priority_queue(new_process);   
    return new_process.pid;
}
   
void initProcessWrapper(program_t program, uint64_t argc, char *argv[]){
    uint64_t ret_value = program(argc, argv);
    if(ret_value != 0){
    }
    current_process.state = TERMINATED;
    __asm__ ("int $0x20");              // TimerTick para llamar a schedule de nuevo
}

void halt(){
    while(1){
        asm_halt();
    }
}

// @returns pcb_t el proceso halt
pcb_t create_process_halt(){
    void *base_pointer = mem_alloc(STACK_SIZE);
    
    if(base_pointer == NULL){
        return (pcb_t){-1, 0, 0, 0, 0, TERMINATED, NULL};
    }

    //TODO: que pongo en argc y argv?
    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, &halt, 0, NULL);
    
    pcb_t new_process = {
                        currentPID++,       //pid
                        stack_pointer,      //rsp
                        0,                  //priority
                        DEFAULT_QUANTUM,    //assigned_quantum
                        0,                  //used_quantum
                        HALT,               //state
                        NULL
                        };
      
    return new_process;
}

void init_scheduler(){
    current_semaphore = 0;
    p0 = new_q();
    p1 = new_q();
    p2 = new_q();
    p3 = new_q();
    all_blocked_queue = new_q();
    current_process = (pcb_t){0, 0, 0, 0, 0, TERMINATED, NULL};   // El primer proceso seria el kernel
}

uint64_t get_pid(){
    return current_process.pid;
}


    // 8 char para el state, 3 para el pid, 1 para un espacio, 1 para el newline
    // buf = mem_alloc(10 + (get_size(process_queue) + get_size(blocked_queue)) * 13);
    // while(has_next(process_queue)){
    //     pcb_t process = dequeue(process_queue);
        
    //     // sprintf(buf, "%s %.8s", "TODO", process.pid, process.state);
    // }
    //  while(has_next(blocked_queue)){
    //     pcb_t process = dequeue(process_queue);
    //     // sprintf(buf, "%s %.8s", "TODO", process.pid, process.state);
    // }



// TODO: Terminar!
void list_processes(char *buf){
    // TODO: Cambiar el tamaño
    // buf = mem_alloc();

    // TODO: Header


    // if(current_process.pid != -1){
    //     add_proccess_to_buffer(current_process, buf);
    // }

    // pcb_t process;
    // q_adt queues[] = {p0, p1, p2, p3, all_blocked_queue};

    // for(int i = 0; i < TOTAL_QUEUES; i++){
    //     q_adt current = queues[i];
    //     while(has_next(current)){
    //         process = dequeue(current);
    //         add_proccess_to_buffer(process, buf);
    //         add(current, process);
    //     }
    // }

    return;
}

// void add_proccess_to_buffer(pcb_t process, char *buf){
//     char *state = get_state_string(process.state);
//     //TODO:lenarlo
// }

// char *get_state_string(int state){
//     char *state_string;
//     switch(state){
//         case READY:
//             state_string = "READY";
//             break;
//         case RUNNING:
//             state_string = "RUNNING";
//             break;
//         case BLOCKED:
//             state_string = "BLOCKED";
//             break;
//         case TERMINATED:
//             state_string = "TERMINATED";
//             break;
//     }
// }

uint64_t kill_process(uint64_t pid){
    pcb_t process;
    if(current_process.pid == pid){
        current_process.state = TERMINATED;
    } else if( (process = find_dequeue_priority(pid)).pid > 0 || (process = find_dequeue_pid(all_blocked_queue, pid)).pid > 0 ){
        mem_free(process.rsp);
    } else {
        return -1;
    }
}

// Bloquea el proceso actual y lo agrega a la cola de procesos bloqueados
uint64_t block_process(){
    current_process.state = BLOCKED;
    __asm__ ("int $0x20");                 // TimerTick para llamar a schedule de nuevo
}

uint64_t block_current_process_to_queue(q_adt blocked_queue){
    current_process.state = BLOCKED;
    add(blocked_queue, current_process);
    __asm__ ("int $0x20");                 // TimerTick para llamar a schedule de nuevo
}

/**
 * @brief Bloquea el proceso con el pid; Lo agrega a la cola que recibe por parametro,
 * y a la cola total de bloqueados.
 *
 * @param pid El identificador del proceso a bloquear.
 * @param queue La cola personalizada a la que se agregará el proceso bloqueado.
 */
uint64_t block_process_to_queue(uint64_t pid, q_adt dest){
    pcb_t process;
    if(current_process.pid == pid){
        block_current_process_to_queue(dest);
    } else if( (process = find_dequeue_priority(pid)).pid > 0){
        process.state = BLOCKED;
        add(dest, process);                    // Agrego el proceso a la cola pasada por parametro         
        add(all_blocked_queue, process);
    } else {
        return -1;
    }
    return 0;
}

/**
 * @brief Desbloquea el primer proceso esperando en la cola recibida por parámetro y lo quita de la cola de all_blocked.
 *
 * Esta función busca el primer proceso en la cola especificada, lo desbloquea y lo elimina de la cola global de procesos bloqueados.
 *
 * @param queue La cola de procesos de la cual se desea desbloquear el primer proceso.
 * @returns 0 en caso de éxito, -1 si no se encontró el proceso en la cola.
 */
uint64_t unblock_process_from_queue(q_adt src){
    pcb_t aux = dequeue(src);    // Pop el primer proceso 

    if(aux.pid < 0){
        return -1;
    }

    // EN LA COLA AUXILIAR NO ESTA EL CONTEXTO DEL PROCESO! NO PUEDO DESBLOQUEARLO USANDO ESE PCB!

    pcb_t process = find_dequeue_pid(all_blocked_queue, aux.pid);       // Ademas lo elimino de la cola de bloqueados

    if(process.pid < 0){
        return -1;
    }

    process.state = READY;
    add_priority_queue(process);
}

// Desbloquea el proceso con el pid dado DE LA COLA ALL_BLOCKED, y lo agrega a la cola de procesos listos
uint64_t unblock_process(uint64_t pid){
    pcb_t process;
    if( (process = find_dequeue_pid(all_blocked_queue, pid)).pid > 0){
        process.state = READY;
        add_priority_queue(process);
    } else {
        return -1;
    }
}

//Elimina un FD de la tabla de FD del proceso actual, a partir del indice en la tabla
//@returns 1 si se elimino correctamente, 0 si no se encontro
int close_file_descriptor_current_process(uint64_t fd_index){
    uint64_t fd_id = current_process.fd_table[fd_index]->id;
    current_process.fd_table[fd_index] = NULL;
    return fd_manager_close(fd_id);
}

void yield(){
    current_process.state = READY;
}

uint8_t add_priority_queue(pcb_t process){
    switch(process.priority){
        case(0):
            add(p0, process);
            break;
        case(1):
            add(p1, process);
            break;
        case(2):
            add(p2, process);
            break;
        case(3):
            add(p3, process);
            break;
        default:
            return -1;
    }
}

/**
 * @brief Busca y popea el proceso en todas las colas de scheduler.c
 * 
 * @return pcb_t el proceso encontrado, o un proceso nulo: (pcb_t){-1, 0, 0, 0, 0, TERMINATED} si no existe
 */
pcb_t find_dequeue_priority(uint64_t pid){
    pcb_t process;
    process = find_dequeue_pid(p0, pid);
    if(process.pid > 0) return process;

    process = find_dequeue_pid(p1, pid);
    if(process.pid > 0) return process;
    
    process = find_dequeue_pid(p2, pid);
    if(process.pid > 0) return process;

    process = find_dequeue_pid(p3, pid);
    if(process.pid > 0) return process;

    return (pcb_t){-1, 0, 0, 0, 0, TERMINATED, NULL};
}
