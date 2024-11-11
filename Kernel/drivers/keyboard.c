#include <stdint.h>
#include <keyboard.h>
#include <pipe.h>
#include <file_descriptor.h>

open_file_t *open_file_t_keyboard;

static const char keyMapL[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 38, 0, '-', 37, 0, 39, '+', 0, 40, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char keyMapU[] = {

    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static const char keyMapCAPS[] = {

    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static const char *const keyMap[] = {keyMapL, keyMapU};

/*
 * 29: left cntrl
 * 42: left shift
 * 54: right shift
 * 55: no tengo numpad
 * 56: left alt
 * 58: caps lock
 * 59 - 68: F1-F10
 * 69: numLock
 * 70: scrollLock
 *
 * Up: 0x48
 * Left: 0x4B
 * Right: 0x4D
 * Down: 0x50
 */

void init_keyboard(){
    open_file_t_keyboard = get_stdin_fd();
}

// Keyboard handler: consigue el scancode del keyboard y escribe al pipe del teclado el ascii code
void keyboard_handler(uint8_t keyPressed){
    unsigned char input_code = 0;
    char ascii_code = 0;
    int shift = 0;
    int caps_lock = 0;
    input_code = keyPressed;

    // shift pressed
    if (input_code == 0x2A || input_code == 0x36)
    {
        shift = 1;
    }
    // shift not pressed
    if (input_code == 0xAA || input_code == 0xB6)
    {
        shift = 0;
    }
    // caps_lock
    if (input_code == 0x3A)
    {
        caps_lock = (caps_lock + 1) % 2;
    }

    if (input_code > 0x80 || input_code == 0x0F)
    {
        ascii_code = 0;
    }
    else if (input_code == 0x48 || input_code == 0x50)
    {
        ascii_code = input_code;
    }
    else
    {
        ascii_code = keyMap[shift][input_code];
    }

    open_file_t_keyboard->write(open_file_t_keyboard->resource, ascii_code);
}

char read_keyboard(){
    return open_file_t_keyboard->read(open_file_t_keyboard->resource);
}   
