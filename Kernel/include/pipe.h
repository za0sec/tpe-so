#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <semaphore.h>

#define BUFF_SIZE 1024
#define SEMAPHORE_NAME_SIZE 32

// Estructura para los pipes
typedef struct pipe {
    uint16_t id;
    char *buffer;
    uint16_t read_index;
    uint16_t write_index;
    char *sem_name_data_available;
    char *sem_name_mutex;
} pipe;

// Inicializa el sistema de pipes, devolviendo 0 si es exitoso o -1 si falla
int init_pipes();

// Crea un pipe y devuelve su ID, o -1 si falla
int pipe_create();

// Destruye el pipe especificado por `pipe_id` y libera sus recursos
// Devuelve 1 si se eliminó exitosamente o 0 si no se encontró
int pipe_destroy(uint16_t pipe_id);

// Lee un char del pipe especificado por `pipe_id`
// Devuelve el carácter leído o -1 si el pipe no existe o la lectura falla
char pipe_read(uint16_t pipe_id);

// Escribe un char `c` en el pipe especificado por `pipe_id`
// Devuelve 0 si se escribió correctamente o -1 si el pipe no existe o la escritura falla
int pipe_write(uint16_t pipe_id, char c);

// Función de comparación de pipes basada en `id`
int compare_pipes(const void *a, const void *b);

// Inicializa un pipe y devuelve un puntero a la estructura, o NULL si falla
pipe *pipe_init();

// Libera la memoria asociada al pipe
void pipe_free(pipe *p);

// Función auxiliar para construir el nombre del semáforo
void get_semaphore_name(const char *base_name, int id, char *dest);

#endif // PIPE_H
