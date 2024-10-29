#include <scheduler.h>
#include <memManager.h>
#include <pcb_queue.h>
#include <utils.h>

uint64_t currentPID = 0;
pcb_t current_process;
q_adt process_queue = NULL;
q_adt blocked_queue = NULL;                 // Procesos bloqueados mediante syscall
q_adt blocked_read_queue = NULL;
q_adt blocked_semaphore_queue = NULL;
uint8_t mutex_lock = 1;

int current_semaphore = 0;


// Retorna -1 por error
uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[]){
    return create_process_state(priority, program, READY, argc, argv);
}

// Retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[]){
    void *base_pointer = mem_alloc(STACK_SIZE);
    if(base_pointer == NULL) return -1;
    // sp += STACK_SIZE;
    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, program, argc, argv);
    pcb_t new_process = {
                        currentPID++,       //pid
                        stack_pointer,      //rsp
                        DEFAULT_QUANTUM,    //assigned_quantum
                        0,                  //used_quantum
                        state               //state
                        };
    add(process_queue, new_process);
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

void create_process_halt(){
    create_process_state(0, &halt, TERMINATED, 0, NULL);
}

void init_scheduler(){
    current_semaphore = 0;
    process_queue = new_q();
    blocked_queue = new_q();
    current_process = (pcb_t){0, 0, DEFAULT_QUANTUM, 0, TERMINATED};   // El primer proceso seria el kernel 
}

uint64_t get_pid(){
    return current_process.pid;
}

void list_processes(char *buf){
    // 8 char para el state, 3 para el pid, 1 para un espacio, 1 para el newline
    buf = mem_alloc(10 + (get_size(process_queue) + get_size(blocked_queue)) * 13);
    while(has_next(process_queue)){
        pcb_t process = dequeue(process_queue);
        
        // sprintf(buf, "%s %.8s", "TODO", process.pid, process.state);
    }
     while(has_next(blocked_queue)){
        pcb_t process = dequeue(process_queue);
        // sprintf(buf, "%s %.8s", "TODO", process.pid, process.state);
    }
}

uint64_t kill_process(uint64_t pid){
    pcb_t process;
    if(current_process.pid == pid){
        current_process.state = TERMINATED;
    } else if( (process = find_dequeue_pid(process_queue, pid)).pid > 0 || (process = find_dequeue_pid(blocked_queue, pid)).pid > 0 ){
        mem_free(process.rsp);
    } else {
        return -1;
    }
}

uint64_t block_process(uint64_t){
    return block_process_to_queue(current_process.pid, blocked_queue);
}

// Bloquea el proceso con el pid, y lo agrega a la cola que recibe por parametro
uint64_t block_process_to_queue(uint64_t pid, q_adt blocked_queue){
    pcb_t process;
    if(current_process.pid == pid){
        current_process.state = BLOCKED;
    } else if( (process = find_dequeue_pid(process_queue, pid)).pid > 0){
        process.state = BLOCKED;
        add(blocked_queue, process);
    } else {
        return -1;
    }
}

// Desbloquea el primer proceso esperando en la cola recibida por parametro
uint64_t unblock_process_from_queue(q_adt blocked_queue){
    pcb_t process = dequeue(blocked_queue);
    process.state = READY;
    add(process_queue, process);
}

// Desbloquea el proceso con el pid dado, y lo agrega a la cola de procesos listos
uint64_t unblock_process(uint64_t pid){
    pcb_t process;
    if( (process = find_dequeue_pid(blocked_queue, pid)).pid > 0){
        process.state = READY;
        add(process_queue, process);
    } else {
        return -1;
    }
}

void yield(){
    current_process.state = READY;
}

uint64_t schedule(void *running_process_rsp){
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
                //Esto deberia ser decrementar LA PRIORIDAD
                current_process.assigned_quantum = CPU_BOUND_QUANTUM;
            }

            add(process_queue, current_process);
        }
    } else if (current_process.state == BLOCKED){
        if(++current_process.used_quantum < current_process.assigned_quantum && current_process.assigned_quantum < IO_BOUND_QUANTUM){
            //Esto deberia ser aumentar LA PRIORIDAD
            current_process.assigned_quantum++;
        }
        add(blocked_queue, current_process);        
    } else {
        //Process is TERMINATED
        mem_free(current_process.rsp);
    } 
    
    if(!has_next(process_queue)){
        //TODO: El halt queda para siempre en la cola
        create_process_halt();
    }
    current_process = dequeue(process_queue);
    current_process.state = RUNNING;

    asm_sti();
    return current_process.rsp;
}

//------------------------------------------------------------------------------
//----------------------------------SEMAPHORES----------------------------------
//------------------------------------------------------------------------------


typedef struct list_t {
    sem_t semaphore;
    struct list_t *next;
} list_t;

list_t *sem_list = NULL;

list_t * add_sem(list_t **head, char * name, int initialValue) {
    list_t * new_node = (list_t *) mem_alloc(sizeof(list_t));
    strcpy(new_node->semaphore.name, name, strlen(name));
    new_node->semaphore.value = initialValue;
    new_node->semaphore.sem_lock = 0;
    new_node->semaphore.blocked_queue = new_q();

    if (*head == NULL) {
        *head = new_node;
    } else {
        list_t *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }

    return new_node;
}

void remove_sem(list_t **head, char * name){
    list_t *temp = *head;
    list_t *prev = NULL;

    if (temp != NULL && strcmp(temp->semaphore.name, name) == 0) {
        *head = temp->next;
        free_q(temp->semaphore.blocked_queue);
        mem_free(temp);
        return;
    }

    while (temp != NULL && strcmp(temp->semaphore.name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    prev->next = temp->next;

    while(has_next(temp->semaphore.blocked_queue)){
        unblock_process_from_queue(temp->semaphore.blocked_queue);   
    }
    free_q(temp->semaphore.blocked_queue);
    mem_free(temp);
}


list_t* find_sem(char * sem_name){
    list_t * aux = sem_list;
    while(aux != NULL){
        if(strcmp(aux->semaphore.name, sem_name) == 0)
            return aux;
        aux = aux->next;
    }
    return NULL;
}

// TODO: Implementar multiples colas de bloqueados, una para cada 'tipo' de bloqueo
// Retorna la direccion de memoria de un nuevo semaforo, o -1 para error

int64_t sem_open(char *sem_id, uint64_t initialValue){
    list_t * aux = find_sem(sem_id);
    if(aux == NULL)
        aux = add_sem(&sem_list, sem_id, initialValue);
    acquire(aux->semaphore.sem_lock);
    aux->semaphore.value++;
    release(aux->semaphore.sem_lock);
    return 0;
}

int64_t sem_close(char * sem_id){
    list_t * aux = find_sem(sem_id);
    if(aux != NULL) {
        acquire(aux->semaphore.sem_lock);
        if(aux->semaphore.value > 0)
            aux->semaphore.value--;
        else
            remove_sem(&sem_list, sem_id);
        release(aux->semaphore.sem_lock);
    }
    return 0;
}

void sem_wait(char * sem_name){
    list_t * sem_node = find_sem(sem_name);
    if(sem_node == NULL) return;
    acquire(sem_node->semaphore.sem_lock);
    if(sem_node->semaphore.value > 0){
        (sem_node->semaphore.value)--;
    } else {
        block_process_to_queue(current_process.pid, sem_node->semaphore.blocked_queue);
    }
    release(sem_node->semaphore.sem_lock);
}

int64_t sem_post(char *sem_id){
    list_t * sem_node;
    if((sem_node = find_sem(sem_id)) == NULL)
        return 1;

    acquire(sem_node->semaphore.sem_lock);
    if (sem_node->semaphore.value == 0){
        unblock_process_from_queue(sem_node->semaphore.blocked_queue);
    }
    (sem_node->semaphore.value)++;
    release(sem_node->semaphore.sem_lock);
    return 0;
}

// void sem_post(sem_t *sem){
//     acquire(sem->sem_lock);
//     if (sem->value == 0){
//         unblock_process_from_queue(sem->blocked_queue);
//     }
//     (sem->value)++;
//     release(sem->sem_lock);       
// }