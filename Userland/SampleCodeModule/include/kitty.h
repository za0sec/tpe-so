#ifndef _KITTY_H_
#define _KITTY_H_

#include <stdio.h>
#include "userlib.h"
#define MAX_BUFF 4096
#define MAX_COMMAND 22
#define MAX_ARGS 25
#define USERNAME_SIZE 16
#define NEW_LINE '\n'
#define BACKSPACE '\b'
#define PLUS '+'
#define MINUS '-'

void kitty();

void printHelp();
void newLine();
void printLine(char c, int username);
void checkLine(int *command_idx, int *after_pipe_idx);
void cmd_undefined();
void cmd_help();
void cmd_time();
void cmd_clear();
void cmd_registersinfo();
void cmd_zeroDiv();
void cmd_invOpcode();
void cmd_exit();
void cmd_charsizeplus();
void cmd_charsizeminus();
void cmd_setusername();
void cmd_ascii();
void printPrompt();
void cmd_whoami();
void cmd_eliminator();
void cmd_memtest();
void historyCaller(int direction);
void handleSpecialCommands(char c);
void cmd_schetest();
void cmd_priotest();
void cmd_testschedulerprocesses();
void cmd_test_sync();
void cmd_ps();
void cmd_cat();
void cmd_loop();

#endif