#include <list.h>
#include <stdlib.h>         // necesito NULL jiji
#include <memManager.h>

// Inicializa una nueva lista y asigna la función de comparación
List *list_init(int (*compare)(const void *, const void *)) {
    List *list = (List *)mem_alloc(sizeof(List));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    list->compare = compare; // Asignamos la función de comparación
    return list;
}

// Agrega un elemento al final de la lista
void list_add(List *list, void *data) {
    ListNode *new_node = (ListNode *)mem_alloc(sizeof(ListNode));
    if (new_node == NULL) {
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (list->head == NULL) { // Si la lista está vacía
        list->head = new_node;
    } else {
        ListNode *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    list->size++;
}

// Elimina el primer elemento que coincide con `data` usando `compare`
// NO HACE FREE DE LA DATA DEL NODO!
int list_remove(List *list, void *data) {
    if (list->head == NULL) { // Lista vacía
        return 0;
    }

    ListNode *current = list->head;
    ListNode *previous = NULL;

    while (current != NULL) {
        if (list->compare(current->data, data) == 0) { // Compara usando `compare`
            if (previous == NULL) { // Si es el primer elemento
                list->head = current->next;
            } else {
                previous->next = current->next;
            }
            mem_free(current);
            list->size--;
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0; // No se encontró el elemento
}

// Obtiene el primer elemento que coincide con `data` usando `compare`
void *list_get(List *list, void *data) {
    ListNode *current = list->head;

    while (current != NULL) {
        if (list->compare(current->data, data) == 0) { // Compara usando `compare`
            return current->data; // Retorna el elemento si `compare` devuelve 0
        }
        current = current->next;
    }
    return NULL; // No se encontró el elemento
}

// Libera la memoria de la lista
void list_free(List *list) {
    ListNode *current = list->head;
    while (current != NULL) {
        ListNode *next = current->next;
        mem_free(current);
        current = next;
    }
    mem_free(list);
}
