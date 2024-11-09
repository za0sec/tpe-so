#include <file_descriptor.h>
#include <scheduler.h>
#include <stdlib.h>
#include <list.h>
#include <memManager.h>
#include <keyboard.h>

List *open_file_descriptors_list;
uint64_t current_id = 0;
open_file_t *STDIN;
open_file_t *STDOUT;

// TODO: syscalls read y write, openfd y closefd

// TODO: Syscall close fd que saque el fd del proceso actual. Si efectivamente estaba presente en 
//       el pcb del proceso actual, llama a fd_manager_close (para bajar su ref_count//eliminarlo)

// TODO: COMPARE! ADEMAS DEPENDE COMO LO IMPLEMENTES, HAY QUE FIJARSE CADA LLAMADO A COMPARE COMO SE ESTA HACIENDO

/*      SYSCALLS
*   - openFD: recibe el id del FD, y lo agrega a la tabla de FD del proceso actual, llamando a fd_manager_open

*   - closeFD: recibe el id del FD en la tabla de FD del proceso actual, y lo elimina de la tabla de FD del proceso actual

*   - readFD: recibe el INDICIE del FD en la tabla de FD del CURRENT PROCESS, y llama a la funcion read del FD:
*             deberia acceder a la PCB del current_process, para conseguir el open_file_descriptor_ID, y llamar
*             a la funcion read del FD.

*   - writeFD: recibe el INDICIE del FD en la tabla de FD del CURRENT PROCESS, y llama a la funcion write del FD:
*/

/*      PIPES
* La idea seria que la misma creacion de pipes cree un FD para dicho pipe, y retorne el id de dicho FD
* Luego, el proceso esta encargado de llamar a la syscall openFD con el id del FD del pipe.
* El llamado a openFD le agrega el FD a su tabla, y retorna el indice de la tabla donde se encuentra el FD, 
* para que el proceso pueda referenciarlo en el futuro.
*/

int compare_file_descriptors(void *this, void *other_fd) {
    //TODO
    return 0;
}

void init_file_descriptors(){
    open_file_descriptors_list = list_init(compare_file_descriptors);
    STDIN = create_fd(read_keyboard, NULL, NULL);
    // STDOUT = TODO: create_fd(write_screen, NULL, NULL);

    if (open_file_descriptors_list == NULL) {
        return -1;
    }

    return 0;
}

open_file_t *get_stdin(){
    return STDIN;
}

open_file_t *get_stdout(){
    return STDOUT;
}

//@returns el indice del FD en la tabla de FD del proceso actual
int fd_manager_open(uint64_t id){
    open_file_t *found_fd = (open_file_t *)list_get(open_file_descriptors_list, id);
    
    if(found_fd == NULL){
        return -1;
    }

    found_fd->ref_count++;

    return add_file_descriptor_current_process(found_fd);
}


int fd_manager_close(uint64_t id){
    open_file_t *found_fd = (open_file_t *)list_get(open_file_descriptors_list, id);
    
    if(found_fd == NULL){
        return -1;
    }

    found_fd->ref_count--;

    if(found_fd->ref_count == 0){
        remove_fd(id);
    }

    remove_file_descriptor_current_process(found_fd);

    return 0;
}

void remove_fd(id){
    open_file_t *found_fd = (open_file_t *)list_get(open_file_descriptors_list, id);
    list_remove(open_file_descriptors_list, found_fd);
    mem_free(found_fd);
}

int add_fd(char (*read)(), char (*write)(char data), int (*close)()){
    open_file_t *new_fd = create_fd(read, write, close);

    if(new_fd == -1){
        return -1;
    }

    list_add(open_file_descriptors_list, new_fd);
    return 1;
}

open_file_t * create_fd(char (*read)(), char (*write)(char data), int (*close)()) {
    open_file_t * new_fd = (open_file_t *)mem_alloc(sizeof(open_file_t));
    
    if(new_fd == NULL){
        return -1;
    }

    new_fd->id = current_id++;
    new_fd->read = read;
    new_fd->write = write;
    new_fd->close = close;
    new_fd->ref_count = 1;

    return new_fd;
}