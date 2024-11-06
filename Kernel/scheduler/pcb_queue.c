// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <pcb_queue.h>
#include <memManager.h>
#include <stdio.h>

typedef struct node * q_t;

typedef struct node{
    pcb_t pcb;
    struct node * next;
} node_t;

typedef struct q_cdt{
    q_t rear;
    size_t size;
} q_cdt;

q_adt new_q() {
    q_adt new = (q_adt)mem_alloc(sizeof(q_cdt));
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

// Agrega un proceso al final de la cola (rear)
void add(q_adt q, pcb_t pcb) {
    q_t aux = (q_t)mem_alloc(sizeof(node_t));
    
    if (aux == NULL) {
        return;
    }

    aux->pcb = pcb;

    if (q->rear == NULL){       // Si la cola está vacía
        aux->next = aux;        // Queda un elemento apuntando a si mismo
    } else {
        aux->next = q->rear->next;
        q->rear->next = aux;
    }

    q->rear = aux;
    q->size++;
}


int has_next(q_adt q){
   return q->rear != NULL;
}

pcb_t dequeue(q_adt q){
    if(q->rear == NULL){
        return (pcb_t){-1, 0, 0, 0, 0, TERMINATED};
    }

    pcb_t to_ret = q->rear->next->pcb;     // El frente de la cola es REAR->NEXT
    
    if(q->rear->next == q->rear){          // La cola tiene un solo elemento
        mem_free(q->rear);               
        q->rear = NULL;
    } else {
        q_t temp = q->rear->next;
        q->rear->next = q->rear->next->next;
        mem_free(temp);
    }
    q->size--;
    return to_ret;
}


pcb_t find_dequeue_pid(q_adt q, uint64_t pid) {
    if (q->rear == NULL) {  // Si la cola está vacía, retornar un PCB "nulo"
        return (pcb_t){-1, 0, 0, 0, 0, TERMINATED};
    }

    q_t current = q->rear; // Comenzamos desde el final
    do{
        if(current->next->pcb.pid == pid){
            pcb_t to_ret = current->next->pcb;
            if(current->next == q->rear){           // Si el proceso a eliminar es el rear
                if(q->rear->next == q->rear){       // Si la cola tiene un solo elemento
                    mem_free(q->rear);
                    q->rear = NULL;
                } else {
                    q_t temp = q->rear;
                    while(temp->next != q->rear){
                        temp = temp->next;
                    }
                    temp->next = q->rear->next;
                    mem_free(q->rear);
                    q->rear = temp;
                }
            } else {
                q_t temp = current->next;
                current->next = current->next->next;
                mem_free(temp);
            }
            q->size--;
            return to_ret;
        }
        current = current->next;
    } while(current != q->rear); 

    // No se encontro el proceso con el pid 
    return (pcb_t){-1, 0, 0, 0, 0, TERMINATED};
}


void free_q(q_adt q) {
    if (q->rear == NULL) {
        mem_free(q);
        return;
    }

    q_t current = q->rear->next; // Comenzamos desde el frente
    q_t next;

    while (current != q->rear) {
        next = current->next;
        mem_free(current);
        current = next;
    }

    // Liberamos el rear
    mem_free(q->rear);
    mem_free(q);
}
