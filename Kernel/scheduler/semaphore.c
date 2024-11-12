// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <scheduler.h>
#include <semaphore.h>
#include <utils.h>
#include <list.h>

List *sem_list;

int compare_semaphores(void *this, void *other_semaphore_name) {
    const sem_t *sem_a = (const sem_t *)this;
    const char *sem_b_name = (char*)other_semaphore_name;
    return strcmp(sem_a->name, sem_b_name);
}

int init_semaphores(){
    sem_list = list_init(compare_semaphores);
    if (sem_list == NULL) {
        return -1;
    }
    return 0;
}

/**
 * @brief Agrega un nuevo semáforo a la lista de semáforos.
 *
 * Esta función crea un nuevo semáforo con el nombre y valor inicial especificados,
 * y lo agrega a la lista de semáforos.
 *
 * @param sem_name El nombre del nuevo semáforo.
 * @param initialValue El valor inicial del semáforo.
 * @return int Retorna 0 si el semáforo fue agregado exitosamente, o -1 si hubo un error en la asignación de memoria.
 */
int add_sem(char * sem_name, int initialValue) {
    sem_t * new_sem = (sem_t *)mem_alloc(sizeof(sem_t));
    if(new_sem == NULL){
        return -1;
    }
    strcpy(new_sem->name, sem_name, strlen(sem_name));
    new_sem->value = initialValue;
    new_sem->sem_lock = (lock_t)1;
    new_sem->blocked_queue = new_q();
    new_sem->count_processes = 1;
    list_add(sem_list, new_sem);

    return 0;
}

/**
 * @brief Elimina un semáforo de la lista de semáforos y libera la memoria asociada.
 *
 * @param sem_name Nombre del semáforo a eliminar.
 */
void remove_sem(char * sem_name){
    sem_t *found_sem = (sem_t *)list_get(sem_list, sem_name);
    list_remove(sem_list, sem_name);
    mem_free(found_sem);
}

// Crea un nuevo semaforo y lo incializa en el valor especificado, si no existia
int sem_open(char *sem_name, uint64_t initialValue){
    sem_t *aux = (sem_t *)list_get(sem_list, sem_name);
    if(aux == NULL){
        return add_sem(sem_name, initialValue);
    } else {
        (aux->count_processes)++;
    }

    return 0;
}

void sem_close(char * sem_name){
    sem_t * aux = (sem_t *)list_get(sem_list, sem_name);
    
    if(aux == NULL){
        return;
    }

    acquire(aux->sem_lock);
    (aux->count_processes)--;
    release(aux->sem_lock);
    
    if(aux->count_processes == 0){
        remove_sem(sem_name);
        return;
    }

    return;
}

void sem_wait(char * sem_name){
    sem_t * sem_node = (sem_t *)list_get(sem_list, sem_name);
    
    if(sem_node == NULL){
        return;
    }

    acquire(sem_node->sem_lock);
    uint8_t need_to_block = sem_node->value <= 0;
    (sem_node->value)--;
    release(sem_node->sem_lock);

    if(need_to_block){
        block_current_process_to_queue(sem_node->blocked_queue);
    }
}

int64_t sem_post(char *sem_name){
    sem_t * sem_node = (sem_t *)list_get(sem_list, sem_name);

    if(sem_node == NULL){
        return 0;
    }

    acquire(sem_node->sem_lock);
    uint8_t need_to_unblock = sem_node->value < 0;
    (sem_node->value)++;
    release(sem_node->sem_lock);
    
    // Desbloqueo el primer proceso bloqueado en la cola del semaforo.
    if (need_to_unblock){
        unblock_process_from_queue(sem_node->blocked_queue);
    }

    return 1;
}
