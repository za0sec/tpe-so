#ifndef OPEN_FILE_T_H
#define OPEN_FILE_T_H

#include <stdint.h>

/**
 * @brief Estructura para representar un descriptor de archivo.
 *
 * La estructura `open_file_t` contiene los punteros a funciones para las operaciones de
 * lectura, escritura y cierre, así como un identificador único y un conteo de referencias
 * para gestionar el recurso de forma compartida entre procesos.
 */
typedef struct {
    uint64_t id;                    // Identificador único del recurso
    void * resource;
    char (*read)(void *resource);
    void (*write)(void *resource, char data);
    int (*close)();                 // Puntero a la función de cierre específico del recurso
    uint32_t ref_count;             // Conteo de referencias para manejo de recursos compartidos
} open_file_t;

#endif // OPEN_FILE_T_H