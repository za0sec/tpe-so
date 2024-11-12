#include <scheduler.h>
#include <stdlib.h>
#include <list.h>
#include <memManager.h>
#include <keyboard.h>
#include <pcb_queue.h>
#include <open_file_t.h>
#include <file_descriptor.h>
#include <videoDriver.h>

List *open_file_descriptors_list;
uint64_t current_fd_id = 0;
uint64_t stdin_fd_id;
open_file_t *open_file_t_stdin;
open_file_t *open_file_t_stdout;
open_file_t *open_file_t_null;

/*      SYSCALLS
*   - openFD: recibe el id del FD, y lo agrega a la tabla de FD del proceso actual, llamando a fd_open_current_process

*   - closeFD: recibe el id del FD en la tabla de FD del proceso actual, y lo elimina de la tabla de FD del proceso actual

*   - readFD: recibe el INDICIE del FD en la tabla de FD del CURRENT PROCESS, y llama a la funcion read del FD:
*             deberia acceder a la PCB del current_process, para conseguir el open_file_descriptor_ID, y llamar
*             a la funcion read del FD.

*   - writeFD: recibe el INDICE del FD en la tabla de FD del CURRENT PROCESS, y llama a la funcion write del FD:
*/

int compare_file_descriptors(void *this_open_file_t, void *other_fd_id) {
    open_file_t *fd_a = (open_file_t *)this_open_file_t;
    uint64_t fd_b_id = (uint64_t)other_fd_id;
    return fd_a->id - fd_b_id;
}

int compare_file_descriptors_id(open_file_t *fd, uint64_t id) {
    return fd->id - id;
}

void init_file_descriptors(){
    open_file_descriptors_list = list_init(compare_file_descriptors);
    stdin_fd_id = pipe_create();                                                                // ID: 0
    open_file_t_stdin = (open_file_t *)list_get(open_file_descriptors_list, stdin_fd_id);       

    open_file_t_stdout = fd_create(0, vDriverRead, vDriverWrite, vDriverClose);                 // ID: 1

    open_file_t_null = fd_create(0, null_read, null_write, null_close);                         // ID: 2
}

char fd_read_current_process(uint64_t process_fd_index){
    pcb_t current_process = get_current_process();
    
    open_file_t *open_file_fd = current_process.fd_table[process_fd_index];

    if(open_file_fd == NULL){
        return -1;
    }

    return open_file_fd->read(open_file_fd->resource);
}

int fd_write_current_process(uint64_t process_fd_index, char data){
    pcb_t current_process = get_current_process();
    
    open_file_t *open_file_fd = current_process.fd_table[process_fd_index];

    if(open_file_fd == NULL){
        return -1;
    }

    return open_file_fd->write(open_file_fd->resource, data);
}

open_file_t * get_stdin_fd(){
    return open_file_t_stdin;
}

open_file_t * get_stdout_fd(){
    return open_file_t_stdout;
}

int null_close(){
    return 1;
}

char null_read(void *resource){
    block_process();
}

int null_write(void *resource, char data){
    return 1;
}

open_file_t * fd_open_fd_table(uint64_t fd_ids[MAX_FD], int fd_count){
    open_file_t **fd_table = (open_file_t **)mem_alloc(sizeof(open_file_t *) * MAX_FD);

    for(int i = 0; i < MAX_FD; i++){
        fd_table[i] = NULL;
    }

    if(fd_ids == NULL || fd_count < 2){
        fd_table[0] = get_stdin_fd();
        fd_table[1] = get_stdout_fd();
    } else {
        if(fd_ids[0] == STDIN){
            fd_table[0] = get_stdin_fd();
        } else if (fd_ids[0] == DEV_NULL){
            fd_table[0] = open_file_t_null;
        } else {
            open_file_t *fd = (open_file_t *)list_get(open_file_descriptors_list, fd_ids[0]);
            
            if(fd == NULL){
                goto undo;
            }

            fd->ref_count++;
            fd_table[0] = fd;
        }
        if(fd_ids[1] == STDOUT){
            fd_table[1] = get_stdout_fd();
        } else if (fd_ids[1] == DEV_NULL){
            fd_table[1] = open_file_t_null;
        } else {
            open_file_t *fd = (open_file_t *)list_get(open_file_descriptors_list, fd_ids[1]);
            
            if(fd == NULL){
                goto undo;
            }

            fd->ref_count++;
            fd_table[1] = fd;
        }
    } 

    for(int i = 2; i < fd_count; i++){
        open_file_t *fd = (open_file_t *)list_get(open_file_descriptors_list, fd_ids[i]);
            
        if(fd == NULL){
            goto undo;
        }

        fd->ref_count++;
        fd_table[i] = fd;
    }

    return fd_table;

    undo:
    for(int i = 0; i < MAX_FD; i++){
        if(fd_table[i] != NULL){
            fd_table[i]->ref_count--;
        }
    }
    return NULL;
}

uint64_t fd_open_current_process(uint64_t fd_id){
    pcb_t current_process = get_current_process();

    // Veo si existe el FD con el fd_id
    open_file_t *found_fd = (open_file_t *)list_get(open_file_descriptors_list, fd_id);

    if(found_fd == NULL){
        return NULL;
    }

    //Chequeo si ya lo tenia abierto
    for(int i = 0; i < MAX_FD; i++){
        if(compare_file_descriptors_id(current_process.fd_table[i], fd_id) == 0){
            return i;
        }
    }

    //Si no se encontro el FD, lo agrego en el primer espacio libre
    for(int i = 0; i < MAX_FD; i++){
        if(current_process.fd_table[i] == NULL){
            current_process.fd_table[i] = found_fd;
            found_fd->ref_count++;
            return i;
        }
    }

    return -1;
}

int fd_close(uint64_t fd_id){
    if(fd_id < 2 || fd_id == stdin_fd_id){
        return 0;
    }

    open_file_t *found_fd = (open_file_t *)list_get(open_file_descriptors_list, fd_id);
    
    if(found_fd == NULL){
        return 0;
    }

    found_fd->ref_count--;

    if(found_fd->ref_count == 0){
        fd_remove(fd_id);
    }

    return 1;
}

int fd_close_current_process(uint64_t fd_index){
    pcb_t current_process = get_current_process();
    open_file_t *fd = current_process.fd_table[fd_index];

    if(fd == NULL){
        return 0;
    }

    uint64_t fd_id = fd->id;
    current_process.fd_table[fd_index] = NULL;
    return fd_close(fd_id);
}

void fd_remove(uint64_t id){
    open_file_t *found_fd = (open_file_t *)list_get(open_file_descriptors_list, id);
    list_remove(open_file_descriptors_list, found_fd);
    mem_free(found_fd);
}

uint64_t fd_add(void *resource, char (*read), int (*write)(char data), int (*close)){
    open_file_t *new_fd = fd_create(resource, read, write, close);

    if(new_fd == NULL){
        return -1;
    }

    list_add(open_file_descriptors_list, new_fd);
    return new_fd->id;
}

open_file_t * fd_create(void *resource, char (*read)(void *src), int (*write)(void *dest, char data), int (*close)()) {
    open_file_t * new_fd = (open_file_t *)mem_alloc(sizeof(open_file_t));
    
    if(new_fd == NULL){
        return NULL;
    }

    new_fd->id = current_fd_id++;
    new_fd->resource = resource;
    new_fd->read = read;
    new_fd->write = write;
    new_fd->close = close;
    new_fd->ref_count = 1;

    return new_fd;
}