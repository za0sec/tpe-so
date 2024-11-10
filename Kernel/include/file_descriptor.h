#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <stdint.h>
#include <list.h>
#include <open_file_t.h>

#define STDIN 0
#define STDOUT 1
#define MAX_FD 10

char fd_read(uint64_t fd_index);

/**
 * @brief Inicializa el sistema de descriptores de archivos.
 *
 * Esta función configura la lista global de descriptores de archivos y crea
 * los descriptores estándar de entrada y salida.
 */
void init_file_descriptors();

/**
 * @brief Devuelve el descriptor de archivo para la entrada estándar (STDIN).
 *
 * @return Puntero al descriptor de archivo STDIN.
 */
open_file_t *get_stdin_fd();

/**
 * @brief Devuelve el descriptor de archivo para la salida estándar (STDOUT).
 *
 * @return Puntero al descriptor de archivo STDOUT.
 */
open_file_t *get_stdout_fd();

/**
 * @brief Abre un descriptor de archivo basado en su ID.
 *
 * @param id El ID del descriptor de archivo a abrir.
 * @return Índice del FD en la tabla del proceso o -1 en caso de error.
 */
uint64_t fd_manager_open(uint64_t fd_id);

/**
 * @brief Cierra un descriptor de archivo basado en su ID.
 *
 * Decrementa el conteo de referencias del descriptor y, si llega a cero,
 * elimina el descriptor de la lista global y libera su memoria.
 *
 * @param id El ID del descriptor de archivo a cerrar.
 * @return 0 en caso de éxito, -1 en caso de error.
 */
int fd_manager_close(uint64_t id);

open_file_t * fd_manager_open_fd_table(uint64_t fd_ids[MAX_FD], int fd_count);

/**
 * @brief Compara dos descriptores de archivo por sus IDs.
 *
 * Esta función se utiliza para buscar un descriptor de archivo en la lista
 * global, comparando los IDs de los descriptores.
 *
 * @param this Puntero al primer descriptor de archivo a comparar.
 * @param other Puntero al segundo descriptor de archivo a comparar.
 * @return Diferencia entre los IDs de los descriptores.
 */
int compare_file_descriptors(void *this, void *other);

int compare_file_descriptors_id(open_file_t *fd, uint64_t id);

/**
 * @brief Crea un nuevo descriptor de archivo.
 *
 * Asigna memoria para un nuevo descriptor de archivo y lo inicializa con las
 * funciones de lectura, escritura y cierre proporcionadas.
 *
 * @param read Puntero a la función de lectura.
 * @param write Puntero a la función de escritura.
 * @param close Puntero a la función de cierre.
 * @return Puntero al nuevo descriptor de archivo, o NULL en caso de error.
 */
open_file_t * fd_create(void *resource, char (*read)(void *src), char (*write)(void *dest, char data), int (*close)());

/**
 * @brief Agrega un nuevo descriptor de archivo al sistema global.
 *
 * Crea un descriptor de archivo usando `fd_create` y lo agrega a la lista
 * global de descriptores.
 *
 * @param read Puntero a la función de lectura.
 * @param write Puntero a la función de escritura.
 * @param close Puntero a la función de cierre.
 * @return 1 en caso de éxito, -1 en caso de error.
 */
int fd_add(void *resource, char (*read)(), char (*write)(char data), int (*close)());

/**
 * @brief Elimina un descriptor de archivo de la lista global.
 *
 * Busca el descriptor por su ID en la lista global, lo elimina y libera
 * la memoria asociada.
 *
 * @param id El ID del descriptor de archivo a eliminar.
 */
void fd_remove(uint64_t id);

#endif // FILE_DESCRIPTOR_H
