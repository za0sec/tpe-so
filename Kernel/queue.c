// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <queue.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct node * q_t;

typedef struct node{
    ;
    size_t size_mb;
    struct node * tail;
} node_t;

typedef struct q_cdt{
    q_t first;
    q_t iterator;
    q_t last;
    int (*compare)(size_t, size_t);
} q_cdt;

q_adt new_q(int (*compare)(size_t, size_t)) {
    q_adt aux = calloc(1, sizeof(q_cdt));
    if (aux == NULL) {
        fprintf(stderr, "Error, no se pudo asignar memoria para la cola.\n");
        return NULL;
    }
    aux->compare = compare;
    return aux;
}

static q_t recursive_add(q_t q, char * path, size_t size_mb, int (*compare)(size_t, size_t)){
    if (q == NULL || compare(q->size_mb, size_mb) >= 0){
        q_t aux = malloc(sizeof(pcb_t));
        if (aux == NULL) {
            fprintf(stderr, "Error, no se pudo asignar memoria para el nuevo q.\n");
            return NULL;
        }
        aux->path = path;
        aux->size_mb = size_mb;
        aux->tail = q;
        return aux;
    }

    if (compare(q->size_mb, size_mb) < 0){
        q->tail = recursive_add(q->tail, path, size_mb, compare);
    }

    return q;
}

void add(q_adt q, char * path, size_t size_mb){
    q_t aux = malloc(sizeof(pcb_t));
    if (aux == NULL) {
        fprintf(stderr, "Error, no se pudo asignar memoria para el nuevo nodo.\n");
        return;
    }
    aux->path = path;
    aux->size_mb = size_mb;
    aux->tail = NULL;

    q->first = recursive_add(q->first, path, size_mb, q->compare);

    free(aux);
}

void to_begin(q_adt q){
    q->iterator = q->first;
}


int has_next(q_adt q){
   return q->iterator != NULL;
}

char * next(q_adt q){
    
    if (has_next(q)){
        char * path = q->iterator->path;
        q->iterator = q->iterator->tail;
        return path;
    }else {
        fprintf(stderr, "No such path!\nAborting...");
        exit(1);
    }
}

void free_q(q_adt q){
    q_t actual = q->first;
    q_t next;

    while(actual != NULL){
        next = actual->tail;
        free(actual);
        actual = next;
    }

    free(q);
}