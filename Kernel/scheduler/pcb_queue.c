// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <pcb_queue.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct node * q_t;

typedef struct node{
    pcb_t pcb;
    struct node * next;
    struct node * prev;
} node_t;

typedef struct q_cdt{
    q_t rear;
} q_cdt;

q_adt new_q() {
    q_adt new = calloc(1, sizeof(q_cdt));
    if (new == NULL) {
        fprintf(stderr, "Error, no se pudo asignar memoria para la cola.\n");
        return NULL;
    }
    return new;
}

void add(q_adt q, pcb_t pcb) {
    q_t aux = malloc(sizeof(node_t));
    if (aux == NULL) {
        fprintf(stderr, "Error, no se pudo asignar memoria para el nuevo nodo.\n");
        return;
    }

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
            free(q->rear);
            q->rear = NULL;
            return pcb;
        }
        q_t aux = q->rear;
        q->rear->prev->next = q->rear->next;
        q->rear->next->prev = q->rear->prev;
        q->rear = q->rear->next;
        return aux->pcb;
    }else {
        fprintf(stderr, "Queue is empty!\nAborting...");
        exit(1);
    }
}

void free_q(q_adt q) {
    while (q->rear != NULL) {
        q_t temp = q->rear->next;  // Nodo a eliminar

        if (q->rear == q->rear->next) {  // Solo un nodo
            free(q->rear);
            q->rear = NULL;
        } else {
            q->rear->next = temp->next;  // Ajustar los punteros
            temp->next->prev = q->rear;
            free(temp);  // Liberar nodo
        }
    }
    free(q);  // Liberar la estructura de la cola
}