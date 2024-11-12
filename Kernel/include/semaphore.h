#ifndef _SEM_H
#define _SEM_H

#include <stdint.h>
#include <scheduler.h>
#include <semaphore.h>
#include <list.h>

#define SEMAPHORE_NAME_SIZE 32

typedef List *sem_list_t;

typedef struct {
    lock_t sem_lock;
    char name[SEMAPHORE_NAME_SIZE];    
    int value;           // Valor del semáforo
    q_adt blocked_queue; // Cola de procesos bloqueados
    int count_processes; // Cantidad de procesos que usan el semáforo
} sem_t;

#define MAX_SEMAPHORES (CHUNK_SIZE / sizeof(sem_t))


// Inicializa la lista de semáforos y devuelve 0 si es exitoso, -1 en caso de error
int init_semaphores(void);

/**
 * @brief Agrega un nuevo semáforo a la lista de semáforos.
 *
 * Esta función crea un nuevo semáforo con el nombre y valor inicial especificados
 * y lo agrega a la lista de semáforos.
 *
 * @param sem_name El nombre del nuevo semáforo.
 * @param initialValue El valor inicial del semáforo.
 * @return int Retorna 0 si el semáforo fue agregado exitosamente, o -1 si hubo un error en la asignación de memoria.
 */
int add_sem(char *sem_name, int initialValue);

/**
 * @brief Elimina un semáforo de la lista de semáforos y libera la memoria asociada.
 *
 * @param sem_name Nombre del semáforo a eliminar.
 */
void remove_sem(char *sem_name);

// Crea un nuevo semáforo e inicializa su valor si no existía previamente
int sem_open(char *sem_name, uint64_t initialValue);

// Cierra un semáforo, reduciendo el contador de procesos que lo usan; si es 0, elimina el semáforo
void sem_close(char *sem_name);

// Decrementa el valor del semáforo; bloquea el proceso actual si el valor es <= 0
void sem_wait(char *sem_name);

// Incrementa el valor del semáforo y desbloquea un proceso en la cola si es necesario
int64_t sem_post(char *sem_name);

/**
 * @brief Compara dos semáforos por nombre para usarse en la lista de semáforos.
 * 
 * @param a El primer semáforo a comparar.
 * @param b El segundo semáforo a comparar.
 * @return int Retorna el resultado de la comparación de cadenas.
 */
int compare_semaphores(void *a, void *b);

#endif // _SEM_H