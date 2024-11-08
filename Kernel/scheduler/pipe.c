#include <semaphore.h>
#include <list.h>
#include <stdlib.h>         // NULL
#include <stdint.h>
#include <memManager.h>
#include <pipe.h>

//tengo que guardar los pipes que existen de alguna manera -> les pongo un id y los guardo en una lista
//ademas mas tarde los pipes van a tener q estar asociados a FD -> guardo el id del pipe en el FD

uint16_t current_id = 0;
List *pipe_list;

int init_pipes(){
    pipe_list = list_init(compare_pipes);\
    if (pipe_list == NULL) {
        return -1;
    }
    return 0;
}

// Retorna el id del pipe creado, o -1 si falla
int pipe_create(){
    pipe *new_pipe = pipe_init();
    if(new_pipe == NULL){
        return -1;
    }
    list_add(pipe_list, new_pipe);
    return new_pipe->id;
}

// Devuelve 0 si se eliminó el pipe, != 0 si no se encontró.
int pipe_destroy(uint16_t pipe_id){
    pipe target_pipe = {.id = pipe_id};
    pipe* found_pipe = (pipe *)list_get(pipe_list, &target_pipe);

    if(found_pipe == NULL){
        return 0;
    }

    int to_ret = list_remove(pipe_list, found_pipe);
    pipe_free(found_pipe);
    
    return to_ret;
}

// Lee un char del pipe. NULL si el pipe no existe.
char pipe_read(uint16_t pipe_id){
    pipe target_pipe = {.id = pipe_id};
    pipe *found_pipe = (pipe *)list_get(pipe_list, &target_pipe);
    if(found_pipe == NULL){
        return NULL;
    }

    sem_wait(found_pipe->sem_name_data_available);
    sem_wait(found_pipe->sem_name_mutex);

    char c = found_pipe->buffer[found_pipe->read_index];
    found_pipe->read_index = (found_pipe->read_index + 1) % BUFF_SIZE;          // Modulo para que se reinicie al llegar la final del array

    sem_post(found_pipe->sem_name_mutex);

    return c;
}

// Escribe un char al pipe. Retorna 0 si se escribió correctamente, -1 si el pipe no se pudo escribir o el pipe no existe.
int pipe_write(uint16_t pipe_id, char c){
    pipe target_pipe = {.id = pipe_id};
    pipe *found_pipe = (pipe *)list_get(pipe_list, &target_pipe);
    if(found_pipe == NULL){
        return -1;
    }

    sem_wait(found_pipe->sem_name_mutex);

    found_pipe->buffer[found_pipe->write_index] = c;
    found_pipe->write_index = (found_pipe->write_index + 1) % BUFF_SIZE;      // Modulo para que se reinicie al llegar la final del array

    sem_post(found_pipe->sem_name_data_available);
    sem_post(found_pipe->sem_name_mutex);

    return 0;

}

// Compare: compara los id`s de dos pipe
int compare_pipes(const void *a, const void *b) {
    const pipe *pipe_a = (const pipe *)a;
    const pipe *pipe_b = (const pipe *)b;
    return (pipe_a->id - pipe_b->id);
}

// Crear una nueva instancia de pipe y su buffer
pipe *pipe_init() {
    pipe *new_pipe = (pipe *)mem_alloc(sizeof(pipe));
    if (new_pipe == NULL) {
        return NULL;
    }

    new_pipe->buffer = (char *)mem_alloc(BUFF_SIZE * sizeof(char));
    if (new_pipe->buffer == NULL) {
        mem_free(new_pipe);
        return NULL;
    }

    new_pipe->id = current_id++;
    new_pipe->read_index = 0;
    new_pipe->write_index = 0;

    new_pipe->sem_name_data_available = mem_alloc(sizeof(char) * SEMAPHORE_NAME_SIZE);
    get_semaphore_name("pipe_sem", new_pipe->id, new_pipe->sem_name_data_available);
    sem_open(new_pipe->sem_name_data_available, 1);
    
    new_pipe->sem_name_mutex = mem_alloc(sizeof(char) * SEMAPHORE_NAME_SIZE);
    get_semaphore_name("pipe_mutex", new_pipe->id, new_pipe->sem_name_mutex);
    sem_open(new_pipe->sem_name_mutex, 1);

    return new_pipe;
}

// Liberar la memoria de un pipe
void pipe_free(pipe *p) {
    if (p == NULL) {
        return;
    }
    mem_free(p->buffer);
    sem_close(p->sem_name_data_available);
    sem_close(p->sem_name_mutex);
    mem_free(p->sem_name_data_available);
    mem_free(p->sem_name_mutex);
    mem_free(p);
}

// Funcion auxiliar: construye el nombre del semáforo en el buffer de destino
void get_semaphore_name(const char *base_name, int id, char *dest)
{
    int base_length = 0;
    while (base_name[base_length] != '\0')
    {
        dest[base_length] = base_name[base_length];
        base_length++;
    }

    char idStr[10];
    intToStr(id, idStr);

    int idIndex = 0;
    while (idStr[idIndex] != '\0')
    {
        dest[base_length++] = idStr[idIndex++];
    }
    dest[base_length] = '\0';
}



//////////////////////////// TESTING ////////////////////////////
#include <videoDriver.h>
// Variables globales para monitorear el estado en GDB
int status_writer1 = 0;
int status_writer2 = 0;
int status_reader = 0;
#define PIPE_ID 0

// Función para escribir en el pipe (writer1)
void pipe_writer1() {
    char c = 'A'; // Dato que este escritor intenta escribir en el pipe
    while (1) {
        vDriver_prints("WRITTEN A", BLACK, WHITE);
        if (pipe_write(PIPE_ID, c) == 0) {
            status_writer1 = 1; // Escritura exitosa
        } else {
            status_writer1 = 0; // Fallo en la escritura
        }
        // Opcional: Añade un pequeño delay aquí si quieres ralentizar el bucle
    }
}

// Función para escribir en el pipe (writer2)
void pipe_writer2() {
    char c = 'B'; // Dato que este escritor intenta escribir en el pipe
    while (1) {
        vDriver_prints("WRITTEN B", WHITE, BLACK);
        if (pipe_write(PIPE_ID, c) == 0) {
            status_writer2 = 1; // Escritura exitosa
        } else {
            status_writer2 = 0; // Fallo en la escritura
        }
        // Opcional: Añade un pequeño delay aquí si quieres ralentizar el bucle
    }
}

// Función para leer del pipe
void pipe_reader() {
    while (1) {
        char c = pipe_read(PIPE_ID);
        vDriver_print(c, WHITE, BLACK); // Imprime el char leído en pantalla\

        if (c != -1) { // -1 indica que la lectura falló o que el pipe no existe
            status_reader = 1; // Lectura exitosa
        } else {
            status_reader = 0; // Fallo en la lectura
        }
        // Opcional: Añade un pequeño delay aquí si quieres ralentizar el bucle
    }
}
    
void pipes_test() {
    // Crea el pipe de prueba (deberia tener id 0 pq es el primero)
    int pipe_id = pipe_create();

    if (pipe_id == -1) {
        // Error al crear el pipe; maneja el error
        return;
    }

    // Crear procesos de prueba: dos procesos de escritura y un proceso de lectura
    // Todos tienen prioridad 1 en este ejemplo, puedes ajustar según lo necesites
    create_process(0, (program_t)pipe_writer1, 0, NULL); // Crea el proceso writer1
    create_process(0, (program_t)pipe_writer2, 0, NULL); // Crea el proceso writer2
    create_process(0, (program_t)pipe_reader, 0, NULL);  // Crea el proceso reader
}
