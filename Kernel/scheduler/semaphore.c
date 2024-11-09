#include <scheduler.h>
#include <semaphore.h>
#include <utils.h>

typedef struct list_t {
    sem_t semaphore;
    struct list_t *next;
} list_t;

list_t *sem_list = NULL;

list_t * add_sem(list_t **head, char * name, int initialValue) {
    list_t * new_node = (list_t *) mem_alloc(sizeof(list_t));
    strcpy(new_node->semaphore.name, name, strlen(name));
    new_node->semaphore.value = initialValue;
    new_node->semaphore.sem_lock = 1;       //TODO: que valor toma al init?
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

// Retorna la direccion de memoria de un nuevo semaforo, o -1 para error
int64_t sem_open(char *sem_id, uint64_t initialValue){
    list_t * aux = find_sem(sem_id);
    if(aux == NULL)
        aux = add_sem(&sem_list, sem_id, initialValue);
    return 0;
}

int64_t sem_close(char * sem_id){
    list_t * aux = find_sem(sem_id);
    if(aux != NULL) {
        acquire(&(aux->semaphore.sem_lock));
        if(aux->semaphore.value > 0)
            aux->semaphore.value--;
        else
            remove_sem(&sem_list, sem_id);
        release(&(aux->semaphore.sem_lock));
    }
    return 0;
}

void sem_wait(char * sem_name){
    list_t * sem_node = find_sem(sem_name);
    
    if(sem_node == NULL){
        return;
    }

    acquire(&(sem_node->semaphore.sem_lock));

    if(sem_node->semaphore.value > 0){
        (sem_node->semaphore.value)--;
        release(&(sem_node->semaphore.sem_lock));
    } else {
        release(&(sem_node->semaphore.sem_lock));
        // Release antes de bloquear, pues al bloquear se cambia de contexto!!
        block_current_process_to_queue(sem_node->semaphore.blocked_queue);
    }
}

int64_t sem_post(char *sem_id){
    list_t * sem_node;
    sem_node = find_sem(sem_id);

    if(sem_node == NULL){
        return 1;
    }

    acquire(&(sem_node->semaphore.sem_lock));
    uint8_t need_to_unblock = sem_node->semaphore.value == 0;
    (sem_node->semaphore.value)++;
    release(&(sem_node->semaphore.sem_lock));
    
    // TODO: Confirmar el orden de las lineas!
    // Desbloqueo el primer proceso bloqueado en la cola del semaforo.
    if (need_to_unblock){
        unblock_process_from_queue(sem_node->semaphore.blocked_queue);
    }

    return 0;
}
