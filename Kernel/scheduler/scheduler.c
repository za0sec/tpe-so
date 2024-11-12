#include <scheduler.h>
#include <memManager.h>
#include <pcb_queue.h>
#include <utils.h>
#include <videoDriver.h>
#include <file_descriptor.h>
#include <open_file_t.h>
#include <list.h>

uint64_t currentPID = 0;
uint64_t foreground_pid = -2;
pcb_t current_process;
pcb_t halt_process;
uint64_t userspace_process_creation_fd_ids[MAX_FD] = {0, 1};
uint64_t userspace_process_creation_fd_count = 0;

uint64_t aging_counter = 0;

q_adt p0 = NULL;
q_adt p1 = NULL;
q_adt p2 = NULL;
q_adt p3 = NULL;

q_adt all_blocked_queue = NULL;                 // Procesos bloqueados mediante syscall
q_adt blocked_read_queue = NULL;
q_adt blocked_semaphore_queue = NULL;

int current_semaphore = 0;
uint8_t mutex_lock = 1;

void init_scheduler(){
    current_semaphore = 0;
    p0 = new_q();
    p1 = new_q();
    p2 = new_q();
    p3 = new_q();
    all_blocked_queue = new_q();
    current_process = return_null_pcb();
    halt_process = create_process_halt();
}

uint64_t schedule(void *running_process_rsp){
    current_process.rsp = running_process_rsp;

    if(++aging_counter >= AGING_THRESHOLD){
        aging_counter = 0;
        apply_aging();
    }

    switch(current_process.state){
        case RUNNING:
            current_process.used_quantum++;

            if(current_process.used_quantum < current_process.assigned_quantum){
                return current_process.rsp;
            } else {
                current_process.state = READY;
                current_process.priority = (current_process.priority + 1) > HIGHEST_QUEUE ? HIGHEST_QUEUE : current_process.priority + 1;
                current_process.assigned_quantum = ASSIGN_QUANTUM(current_process.priority);
                current_process.used_quantum = 0;
                add_priority_queue(current_process);
            }
            break;

        case BLOCKED:
            // Si el proceso se bloquea antes de usar sus quantums, lo paso a una cola de mejor prioridad
            if(++current_process.used_quantum < current_process.assigned_quantum){
                current_process.priority = (current_process.priority - 1) < 0 ? 0 : current_process.priority - 1;
                current_process.assigned_quantum = ASSIGN_QUANTUM(current_process.priority);
            }

            current_process.used_quantum = 0;

            add(all_blocked_queue, current_process);
            break;

        case READY:
            add_priority_queue(current_process);
            break;

        case HALT:
            halt_process.rsp = running_process_rsp;
            break;

        case TERMINATED:
            if(current_process.pid == foreground_pid){
                current_process.fd_table[1]->write(current_process.fd_table[1]->resource, -1);
                foreground_pid = -1;
            }
        
            if(current_process.fd_table != NULL){
                for(int i = 0; i < MAX_FD; i++){
                    if(current_process.fd_table[i] != NULL){
                        fd_close(current_process.fd_table[i]->id);
                    }
                }
            }
            
            mem_free(current_process.base_sp);   
    
            while(has_next(current_process.waiting_list)){
                unblock_process_from_queue(current_process.waiting_list);
            }

            break;

        default:            // se estaba ejecutando el proceso halt
            break;
    }

    pcb_t next_process = get_next_process();
    
    if(next_process.state == READY){
        current_process = next_process;
        current_process.state = RUNNING;
    } else {
        current_process = halt_process;
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
        return return_null_pcb();
    }
    return next_process;
}

pcb_t get_current_process(){
    return current_process;
}

void apply_aging(){
    q_adt queues[] = {p0, p1, p2, p3};
    pcb_t process;
    for (int i = 1; i <= HIGHEST_QUEUE; i++) {
        while (has_next(queues[i])) {
            process = dequeue(queues[i]);
            process.priority = i - 1;
            add_priority_queue(process);
        }
    }
}

uint64_t userspace_create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[]){
    foreground_pid = create_process_state(priority, program, READY, argc, argv, userspace_process_creation_fd_ids, userspace_process_creation_fd_count);
    return foreground_pid;
}

uint64_t userspace_create_process(int priority, program_t program, uint64_t argc, char *argv[]){
    return create_process_state(priority, program, READY, argc, argv, userspace_process_creation_fd_ids, userspace_process_creation_fd_count);
}

// Retorna -1 por error
uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[MAX_FD], uint64_t fd_count){
    return create_process_state(priority, program, READY, argc, argv, fd_ids, fd_count);
}

// Si le pasas fd_count < 2, se le asignan los fd de stdin y stdout!
// Retorna -1 por error
uint64_t create_process_state(int priority, program_t program, int state, uint64_t argc, char *argv[], uint64_t fd_ids[MAX_FD], uint64_t fd_count){    
    void *base_pointer = mem_alloc(STACK_SIZE);
    
    if(base_pointer == NULL){
        return -1;
    }

    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, program, argc, argv);

    q_adt waiting_list = new_q();
    
    open_file_t **fd_table = fd_open_fd_table(fd_ids, fd_count);

    int assigned_quantum = ASSIGN_QUANTUM(priority);  // Quantum segun nivel de prioridad

    pcb_t new_process = {
                        currentPID++,       //pid
                        base_pointer,       //base_sp
                        stack_pointer,      //rsp
                        priority,           //priority
                        assigned_quantum,    //assigned_quantum
                        0,                  //used_quantum
                        state,              //state
                        fd_table,
                        waiting_list        //waiting_list
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

void userspace_set_fd(int *fd_ids, int fd_count){
    if(fd_count < 2){
        userspace_process_creation_fd_ids[0] = 0;
        userspace_process_creation_fd_ids[1] = 1;
        userspace_process_creation_fd_count = 0;
        return;
    }

    int fd_idx;
    for(fd_idx = 0; fd_idx < fd_count; fd_idx++){
        userspace_process_creation_fd_ids[fd_idx] = fd_ids[fd_idx];
    }
    for(; fd_idx < MAX_FD; fd_idx++){
        userspace_process_creation_fd_ids[fd_idx] = 2;
    }
    userspace_process_creation_fd_count = fd_count;
}

// @returns pcb_t el proceso halt
pcb_t create_process_halt(){
    void *base_pointer = mem_alloc(STACK_SIZE);
    
    if(base_pointer == NULL){
        return return_null_pcb();
    }

    void * stack_pointer = fill_stack(base_pointer, initProcessWrapper, &halt, 0, NULL);

    pcb_t new_process = {
                        currentPID++,       //pid
                        base_pointer,       //base_sp
                        stack_pointer,      //rsp
                        0,                  //priority
                        0,                  //assigned_quantum
                        0,                  //used_quantum
                        HALT,               //state
                        NULL,               //fd_table
                        NULL                //waiting_list
                        };
      
    return new_process;
}

uint64_t get_pid(){
    return current_process.pid;
}

uint64_t kill_process_foreground(){
    if(foreground_pid < 0){
        return -1;
    }
    uint64_t aux = foreground_pid;
    kill_process(foreground_pid);
    foreground_pid = -2;
    return aux;
}

uint64_t set_priority(uint64_t pid, uint8_t priority){
    priority = priority % HIGHEST_QUEUE;
    pcb_t process;
    if(current_process.pid == pid){
        current_process.priority = priority;
        current_process.assigned_quantum = ASSIGN_QUANTUM(priority);
    } else if( (process = find_dequeue_priority(pid)).pid > 0 ){
        process.priority = priority;
        add_priority_queue(process);
    } else {
        return -1;
    }
}

uint64_t kill_process(uint64_t pid){
    pcb_t process;
    if(current_process.pid == pid){
        current_process.state = TERMINATED;
        __asm__ ("int $0x20");                 // TimerTick para llamar a schedule de nuevo
        return 1;
    } else if( (process = find_dequeue_priority(pid)).pid > 0 || (process = find_dequeue_pid(all_blocked_queue, pid)).pid > 0 ){
        mem_free(process.base_sp);
        
        if(process.fd_table != NULL){
            for(int i = 0; i < MAX_FD; i++){
                if(process.fd_table[i] != NULL){
                    fd_close(process.fd_table[i]->id);
                }
            }
        }
    
        while(has_next(process.waiting_list)){
            unblock_process_from_queue(process.waiting_list);
        }

        return 1;
    } else {
        return 0;
    }
}

uint64_t kill_process_terminal(char * pid){
    return kill_process(atoi(pid));
}

void block_process_pid(uint64_t pid){
    if(current_process.pid == pid){
        return;
    }

    pcb_t process = find_dequeue_priority(pid);

    if(process.pid < 0){
        return;
    }

    process.state = BLOCKED;
    add(all_blocked_queue, process);
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

uint64_t unblock_process(uint64_t pid){
    pcb_t process;
    if( (process = find_dequeue_pid(all_blocked_queue, pid)).pid > 0){
        process.state = READY;
        add_priority_queue(process);
    } else {
        return -1;
    }
}

void yield(){
    current_process.state = READY;
}

uint8_t add_priority_queue(pcb_t process){
    process.assigned_quantum = ASSIGN_QUANTUM(process.priority);  // Quantum segun nivel de prioridad
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

    return return_null_pcb();
}


int find_process_in_priority_queues(uint64_t pid) {
    q_adt queues[] = {p0, p1, p2, p3};
    for (int i = 0; i < 4; i++) {
        if (find_process_in_queue(queues[i], pid)) {
            return 1;
        }
    }
    return 0;
}

int find_process_in_queue(q_adt q, uint64_t pid) {
    size_t size = get_size(q);
    int found = 0;
    pcb_t process;

    for (size_t i = 0; i < size; i++) {
        process = dequeue(q);
        if (process.pid == pid && process.state != TERMINATED) {
            found = 1;
        }
        // Reagrega el proceso a la colita
        add(q, process);
        if (found) {
            break;
        }
    }

    return found;
}


int is_valid_pid(uint64_t pid){
    if (current_process.pid == pid && current_process.state != TERMINATED){
        return 1;
    }

    if (find_process_in_priority_queues(pid)){
        return 1;
    }

    if (find_process_in_queue(all_blocked_queue, pid)) {
        return 1;
    }

    return 0;
}

pcb_t get_process_by_pid(uint64_t pid) {
    q_adt queues[] = {p0, p1, p2, p3, all_blocked_queue, NULL};
    pcb_t process;

    for(int i = 0; queues[i] != NULL; i++) {
        process = find_dequeue_pid(queues[i], pid);
        if(process.pid > 0) {
            add(queues[i], process);
            return process;
        }
    }

    return return_null_pcb();
}


void wait_pid(uint64_t pid) {
    if (!is_valid_pid(pid)) {
        // El PID esta mal o ya termino!!
        return;
    }
    pcb_t process = get_process_by_pid(pid);
    block_current_process_to_queue(process.waiting_list);
}


char *list_processes() {
    // Tamaño aproximado de cada línea (PID + PRIORITY + STATE + BASE POINTER + separadores y newline)
    const int line_size = 68;
    const int header_size = 68; // tamaño aproximado del header
    char *header = "PID PRIORITY STATE    BASE POINTER\n";
    
    // Calcula el tamaño necesario para almacenar todos los procesos
    int total_processes = 1; // Incluir el proceso actual
    pcb_t process;
    q_adt queues[] = {p0, p1, p2, p3, all_blocked_queue};
    
    // Calcula la cantidad total de procesos en todas las colas
    for (int i = 0; i < TOTAL_QUEUES; i++) {
        total_processes += get_size(queues[i]);
    }
    
    // Asigna memoria para el buffer completo
    char *buffer = mem_alloc(header_size + (line_size * total_processes));
    int offset = 0;
    
    // Copia el header al buffer
    strcpy(buffer, header, header_size);
    offset += strlen(header);

    // Agrega el proceso actual si está definido
    if (current_process.pid != -1) {
        char line[line_size];
        format_process_line(line, &current_process);
        strcpy(buffer + offset, line, line_size);
        offset += strlen(line);
    }

    // Procesa cada cola de procesos
    for (int i = 0; i < TOTAL_QUEUES; i++) {
        q_adt current = queues[i];
        int size = get_size(current);

        // Agrega cada proceso de la cola al buffer
        for (int j = 0; j < size; j++) {
            process = dequeue(current);

            // Formatear cada proceso en una línea
            char line[line_size];
            format_process_line(line, &process);

            // Copiar la línea al buffer final
            strcpy(buffer + offset, line, line_size);
            offset += strlen(line);

            // Reencolar el proceso en su cola original
            add(current, process);
        }
    }
    return buffer;
}

void format_process_line(char *line, pcb_t *process) {
    char pid_str[10];
    char priority_str[5];
    char base_pointer_str[10];
    char *state_str;

    // Convierte los enteros a cadena
    intToStr(process->pid, pid_str);
    intToStr(process->priority, priority_str);
    intToStr(process->base_sp, base_pointer_str);

    // Determina la cadena de estado
    switch (process->state) {
        case READY: state_str = "READY"; break;
        case RUNNING: state_str = "RUNNING"; break;
        case BLOCKED: state_str = "BLOCKED"; break;
        case TERMINATED: state_str = "TERMINATED"; break;
        case HALT: state_str = "HALT"; break;
    }

    // Construye la línea en el formato deseado: "PID   PRIORITY   STATE\n"
    int offset = 0;

    // Copia el PID en la línea
    strcpy(line + offset, pid_str, strlen(pid_str));
    offset += strlen(pid_str);
    for(int i = 0; i < 3; i++){
        line[offset++] = ' ';
    }

    // Copia la PRIORIDAD en la línea
    strcpy(line + offset, priority_str, strlen(priority_str));
    offset += strlen(priority_str);
    for(int i = 0; i < 8; i++){
        line[offset++] = ' ';
    }

    // Copia el ESTADO en la línea
    strcpy(line + offset, state_str, strlen(state_str));
    offset += strlen(state_str);
    for(int i = 0; i < 2; i++){
        line[offset++] = ' ';
    }

    // Copia el BASE POINTER en la línea
    strcpy(line + offset, base_pointer_str, strlen(base_pointer_str));
    offset += strlen(base_pointer_str);
    line[offset++] = ' ';

    // Añade el carácter de nueva línea
    line[offset++] = '\n';
    line[offset] = '\0'; // Termina la cadena
}