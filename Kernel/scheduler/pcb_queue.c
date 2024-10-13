// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <pcb_queue.h>
#include <memManager.h>
#include <stdio.h>

typedef struct node * q_t;

typedef struct node{
    pcb_t pcb;
    struct node * next;
    struct node * prev;
} node_t;

typedef struct q_cdt{
    q_t rear;
    size_t size;
} q_cdt;

q_adt new_q() {
    q_adt new = mem_alloc(sizeof(q_cdt));
    if (new == NULL) {
        return NULL;
    }
    new->size = 0;
    new->rear = NULL;
    return new;
}

size_t get_size(q_adt q) {
    return q->size;
}

void add(q_adt q, pcb_t pcb) {
    q_t aux = mem_alloc(sizeof(node_t));
    if (aux == NULL) {
        return;
    }

    q->size++;
    aux->pcb = pcb;

    if (q->rear == NULL){
        q->rear = aux;
        q->rear->next = q->rear;
        q->rear->prev = q->rear;
        return;
    }

    q_t rear = q->rear;
    rear->prev->next = aux;
    aux->prev = rear->prev;
    rear->prev = aux;
    aux->next = rear;
}

int has_next(q_adt q){
   return q->rear != NULL;
}

pcb_t dequeue(q_adt q){
    if (has_next(q)){ 
        if (q->rear == q->rear->next) {  // Solo un nodo en la cola
            pcb_t pcb = q->rear->pcb;
            mem_free(q->rear);
            q->rear = NULL;
            return pcb;
        }
        q_t aux = q->rear;
        q->rear->prev->next = q->rear->next;
        q->rear->next->prev = q->rear->prev;
        q->rear = q->rear->next;
        q->size--;
        pcb_t pcb = aux->pcb;            // Almacenar el PCB antes de liberar
        mem_free(aux);                   // Liberar el nodo eliminado
        return aux->pcb;
    }else {
        return (pcb_t){0, 0, 0, 0, TERMINATED};
    }
}

pcb_t find_dequeue_pid(q_adt q, uint64_t pid) {
    if (q->rear == NULL) {  // Si la cola está vacía, retornar un PCB "nulo"
        return (pcb_t){0, 0, 0, 0, TERMINATED};
    }

    q_t current = q->rear;
    do {
        if (current->pcb.pid == pid) {
            // Si encontramos el proceso con el pid correspondiente

            pcb_t found_pcb = current->pcb;

            if (current == q->rear && q->rear->next == q->rear) {
                // Solo un nodo en la cola y es el que buscamos
                q->rear = NULL;
            } else {
                // Ajustamos los punteros para eliminar el nodo
                current->prev->next = current->next;
                current->next->prev = current->prev;

                // Si el nodo que estamos eliminando es el rear, lo actualizamos
                if (current == q->rear) {
                    q->rear = current->next;
                }
            }

            q->size--;
            mem_free(current);  // Liberamos la memoria del nodo eliminado
            return found_pcb;   // Retornamos el PCB del proceso encontrado
        }

        current = current->next;  // Pasamos al siguiente nodo
    } while (current != q->rear);  // Seguimos mientras no volvamos al inicio

    // Si no encontramos el proceso con el pid dado, retornamos un PCB "nulo"
    return (pcb_t){-1, 0, 0, 0, TERMINATED};
}


void free_q(q_adt q) {
    while (q->rear != NULL) {
        q_t temp = q->rear->next;  // Nodo a eliminar

        if (q->rear == q->rear->next) {  // Solo un nodo
            mem_free(q->rear);
            q->rear = NULL;
        } else {
            q->rear->next = temp->next;  // Ajustar los punteros
            temp->next->prev = q->rear;
            mem_free(temp);  // Liberar nodo
        }
    }
    mem_free(q);  // Liberar la estructura de la cola
}