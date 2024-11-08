#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Inicializa el pipe del teclado
void init_keyboard();

// Manejador del teclado: recibe el scancode del teclado y escribe el código ASCII en el pipe del teclado
void keyboard_handler(uint8_t keyPressed);

// Lee un carácter ASCII desde el pipe del teclado
char read_keyboard();

#endif // KEYBOARD_H
