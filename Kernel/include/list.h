#ifndef _LIST_H_
#define _LIST_H_

#include <stdint.h>

// Definición de la estructura de nodo para la lista
typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

// Definición de la estructura de la lista
typedef struct {
    ListNode *head;
    uint64_t size;
    int (*compare)(void *, void *); // Puntero a la función de comparación
} List;

// Inicializa una nueva lista, recibiendo un puntero a la función de comparación
List *list_init(int (*compare)(void *, void *));

// Agrega un elemento al final de la lista
void list_add(List *list, void *data);

// Elimina el primer elemento que coincide con `data` utilizando `compare`
// Devuelve 1 si se eliminó el elemento, 0 si no se encontró
int list_remove(List *list, void *data);

// Obtiene el primer elemento que coincide con `data` utilizando `compare`
// Devuelve NULL si no se encontró
void *list_get(List *list, void * data);

// Libera la memoria de la lista
void list_free(List *list);

#endif // _LIST_H_
