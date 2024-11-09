#ifndef __queue
#define __queue
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct pcb {
    int pid;
    void *rsp;
    uint8_t priority;
    uint8_t assigned_quantum;
    uint8_t used_quantum;
    enum {
        READY,
        RUNNING,
        BLOCKED,
        TERMINATED,
        HALT
    } state;
} pcb_t;

// Estructura para manejar la cola
typedef struct q_cdt * q_adt;

size_t get_size(q_adt q);

/** 
 * @return El proceso buscado, si no existe retorna un proceso nulo: (pcb_t){-1, 0, 0, 0, TERMINATED}; 
 */
pcb_t find_dequeue_pid(q_adt q, uint64_t pid);

/**
 * @brief Crea una nueva cola circular doblemente enlazada
 * 
 * @return q_adt La cola inicializada, o NULL si hubo error
 */
q_adt new_q();

/**
 * @brief Agrega un proceso (pcb_t) a la cola circular
 * 
 * @param q La cola en la que se agregará el proceso
 * @param pcb El proceso a agregar a la cola
 */
void add(q_adt q, pcb_t pcb);

/**
 * @brief Verifica si la cola tiene más nodos para procesar
 * 
 * @param q La cola
 * @return int 1 si hay nodos en la cola, 0 si no
 */
int has_next(q_adt q);

/**
 * @brief Elimina el proceso más antiguo de la cola y lo devuelve
 * 
 * @param q La cola de la que se elimina un proceso
 * @return pcb_t El proceso eliminado
 */
pcb_t dequeue(q_adt q);

/**
 * @brief Libera la memoria de toda la cola y sus nodos
 * 
 * @param q La cola a liberar
 */
void free_q(q_adt q);

#endif // __queue
