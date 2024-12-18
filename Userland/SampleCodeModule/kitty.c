// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <userlib.h>
#include <stdio.h>
#include <time.h>
#include <sys_calls.h>
#include <colors.h>
#include <eliminator.h>
#include <kitty.h>
#include <ascii.h>
#include <tests.h>
#include <philo.h>
// initialize all to 0
char line[MAX_BUFF + 1] = {0};
char parameter[MAX_BUFF + 1] = {0};
char after_pipe[MAX_BUFF + 1] = {0};
char command[MAX_BUFF + 1] = {0};
int command_idx = 0;
int after_pipe_idx = 0;
int terminate = 0;
int linePos = 0;
char lastc;
static char username[USERNAME_SIZE] = "user";
static char commandHistory[MAX_COMMAND][MAX_BUFF] = {0};
static int commandIterator = 0;
static int commandIdxMax = 0;
static int run_in_background = 0;
uint64_t cursor_pid;

#define ASCII_ART_COUNT 10
#define MAX_ASCII_HEIGHT 10

char usernameLength = 4;


// Forward declarations
void newLineUsername();
int isUpperArrow(char c);
int isDownArrow(char c);

const char *commands[] = {"undefined", "help", "ls", "time", "clear", "registersinfo", "zerodiv",
                         "invopcode", "setusername", "whoami", "exit", "ascii", "eliminator", "memtest",
                         "schetest","priotest", "runtestprocesses", "testsync", "ps", "cat", "loop", 
						 "kill", "philo", "wc", "filter", "block", "unblock", "nice", "mem", "testchildren"};

static program_t commands_ptr[MAX_ARGS] = {cmd_undefined, cmd_help, cmd_help, cmd_time, cmd_clear, cmd_registersinfo, cmd_zeroDiv,
                                          cmd_invOpcode, cmd_setusername, cmd_whoami, cmd_exit, cmd_ascii, cmd_eliminator, cmd_memtest,
                                          cmd_schetest, cmd_priotest, cmd_run_test_processes, cmd_test_sync, cmd_ps, cmd_cat, cmd_loop, 
										  cmd_kill, cmd_philo, cmd_wc, cmd_filter, cmd_block, cmd_unblock, cmd_nice, cmd_mem, cmd_test_children};

void kitty(){
	welcome();
	char c;
	printPrompt();
	while (1 && !terminate)
	{
		c = getChar();
		printLine(c, strcmp(username, "user"));
	}
}

void printLine(char c, int username)
{
	if (linePos >= MAX_BUFF || c == lastc)
	{
		return;
	}
	if (isChar(c) || c == ' ' || isDigit(c))
	{
		handleSpecialCommands(c);
	}
	else if (c == BACKSPACE && linePos > 0)
	{
		printc(c);
		line[--linePos] = 0;
	}
	else if (c == NEW_LINE && username)
	{
		process_line();
		prints("\n", MAX_BUFF);
		printPrompt();
	}
	else if (c == NEW_LINE && !username)
	{
		newLineUsername();
	}
	lastc = c;
}

void process_line(){
	checkLine(&command_idx, &after_pipe_idx);

	prints("\n", MAX_BUFF);

	if (command_idx && after_pipe_idx){
		pipe_command();
	} else if (command_idx && run_in_background){
		run_in_background = 0;
		uint64_t fd_table[10] = {0};
		fd_table[0] = 2;
		fd_table[1] = 2;
		create_process(0, commands_ptr[command_idx], 0, NULL, fd_table, 2);
	} else if (command_idx){
		uint64_t pid = create_process_foreground(0, commands_ptr[command_idx], 0, NULL, NULL, 0);
		sys_wait_pid(pid);
	}

	for (int i = 0; line[i] != '\0'; i++){
		line[i] = 0;
		command[i] = 0;
		parameter[i] = 0;
		after_pipe[i] = 0;
		command_idx = 0;
		after_pipe_idx = 0;

	}
	linePos = 0;
}

void pipe_command(){
	uint64_t fd_id = sys_pipe_create();
	uint64_t first_pipe_fds[10] = {0};
	uint64_t second_pipe_fds[10] = {0};
	first_pipe_fds[1] = fd_id;
	second_pipe_fds[0] = fd_id;
	second_pipe_fds[1] = 1;				// stdout
	
	uint64_t pid1 = create_process_foreground(0, commands_ptr[command_idx], 0, NULL, first_pipe_fds, 2);
	uint64_t pid2 = create_process(0, commands_ptr[after_pipe_idx], 0, NULL, second_pipe_fds, 2);
	
	sys_wait_pid(pid1);
	sys_wait_pid(pid2);
}

void printPrompt()
{
	prints(username, usernameLength);
	prints(" $", MAX_BUFF);
	printcColor('>', PINK);
}

// separa comando de parametro
void checkLine(int *command_idx, int *after_pipe_idx){
	int i = 0;
	int j = 0;
	int m = 0;
	int k = 0;

	for (j = 0; j < linePos && line[j] != ' '; j++){
		command[j] = line[j];
	}

	while (j < linePos){
		j++;
		if (line[j] == '&'){
			run_in_background = 1;
			break;
		} else if (line[j] == '|'){
			while (line[j] == ' ' || line[j] == '|') j++;

			while (j < linePos) {
				after_pipe[m++] = line[j++];
			}
		} else {
			parameter[k++] = line[j++];
		}
	}

	strcpyForParam(commandHistory[commandIdxMax++], command, parameter);
	commandIterator = commandIdxMax;

	for (i = 1; i < MAX_ARGS; i++) {
		if (strcmp(command, commands[i]) == 0) {
			*command_idx = i;
			if (after_pipe[0] == '\0') {
				return;
			}
			for (int j = 1; j < MAX_ARGS; j++) {
				if (strcmp(after_pipe, commands[j]) == 0) {
					*after_pipe_idx = j;
					return;
				}
			}
		}
	}
}

uint64_t cmd_loop(uint64_t argc, char *argv[]){
	uint64_t pid = sys_getPID();
	char * pid_str = sys_mem_alloc(10);
	intToStr(pid, pid_str);
	int flag = 0;
	while (1) {
    	if (flag && sys_getSeconds() % 5 == 0){
			write_string("Hola, soy el proceso ", strlen("Hola, soy el proceso"));
			write_string(pid_str, strlen(pid_str));
			write_string("\n", strlen("\n"));
			flag = 0;
		} else {
			flag = 1;
		}
  	}
	sys_mem_free(pid_str);
	return 0;
}

uint64_t cmd_kill(uint64_t argc, char *argv[]){
	char * pid_str = parameter;
	uint64_t pid = str_to_int(parameter);
	if (sys_kill(pid)){
		write_string("Killed process with PID: ", MAX_BUFF);
		write_string(pid_str, MAX_BUFF);
		write_string("\n", MAX_BUFF);
	}
	return 0;
}

uint64_t cmd_setusername(uint64_t argc, char *argv[]){
	int input_length = strlen(parameter);
	if (input_length < 3 || input_length > USERNAME_SIZE){
		prints("\nERROR: Username length must be between 3 and 16 characters long! Username not set.", MAX_BUFF);
		return -1;
	}
	usernameLength = input_length;
	for (int i = 0; i < input_length; i++){
		username[i] = parameter[i];
	}
	prints("\nUsername set to ", MAX_BUFF);
	prints(username, usernameLength);
	return 0;
}

uint64_t cmd_whoami(uint64_t argc, char *argv[]){
	prints("\n", MAX_BUFF);
	prints(username, usernameLength);
	return 0;
}

uint64_t cmd_memtest(uint64_t argc, char *argv[]){
    char *arg[] = {"100000000000000"};
    if (test_mm(1, arg) == -1){
		printsColor("test_mm ERROR\n", MAX_BUFF, RED);
	}
	return 0;
}

uint64_t cmd_cat(uint64_t argc, char *argv[]){
	cat();
	return 0;
}
uint64_t cmd_schetest(uint64_t argc, char *argv[]){
    char *arg[] = {"3"};
	create_process(1, (program_t)test_processes, 1, arg, 0, 0);
	return 0;
}

uint64_t cmd_priotest(uint64_t argc, char *argv[]){
	test_prio();
	return 0;
}

uint64_t cmd_help(uint64_t argc, char *argv[]){
	// write_string("\n\n===== Listing a preview of available PIBES commands =====\n", MAX_BUFF);
	// write_string("\n\n    >'help' or 'ls'     - displays this shell information", MAX_BUFF);
	// write_string("\n    >whoami             - display current username", MAX_BUFF);
	// write_string("\n    >time               - display current time", MAX_BUFF);
	// write_string("\n    >clear              - clear the display", MAX_BUFF);
	// write_string("\n    >(+)                - increase font size (scaled)", MAX_BUFF);
	// write_string("\n    >(-)                - decrease font size (scaled)", MAX_BUFF);
	// write_string("\n    >registersinfo      - print current register values", MAX_BUFF);
	// write_string("\n    >zerodiv            - testeo divide by zero exception", MAX_BUFF);
	// write_string("\n    >invopcode          - testeo invalid op code exception", MAX_BUFF);
	// write_string("\n    >whoami             - prints current username", MAX_BUFF);
	// write_string("\n    >memtest            - test memory manager", MAX_BUFF);
	// write_string("\n    >schetest           - test scheduler", MAX_BUFF);
	// write_string("\n    >priotest           - priority scheduler", MAX_BUFF);
	// write_string("\n    >runtestprocesses   - run test processes", MAX_BUFF);
	// write_string("\n    >testsync           - test synchro", MAX_BUFF);
	// write_string("\n    >ps                 - list all processes", MAX_BUFF);
	// write_string("\n    >cat                - cat file", MAX_BUFF);
	// write_string("\n    >loop               - prints short greeting and process PID", MAX_BUFF);
	// write_string("\n    >kill [PID]         - kill specified process", MAX_BUFF);
	// write_string("\n    >block [PID]        - block specified process", MAX_BUFF);
	// write_string("\n    >unblock [PID]      - unblock specified process", MAX_BUFF);
	// write_string("\n    >nice [PID] [prio]  - change a given's process priority", MAX_BUFF);
	// write_string("\n    >philo              - test philosophers", MAX_BUFF);
	// write_string("\n    >wc                 - counts the total amount of input lines", MAX_BUFF);
	// write_string("\n    >filter             - filt all input vocals", MAX_BUFF);
	// write_string("\n    >mem                - print memory state\n", MAX_BUFF);
	// write_string("\n    >testchildren                - test children processes\n", MAX_BUFF);
	// write_string("\n    >exit               - exit PIBES OS\n\n", MAX_BUFF);
	char *help = "===== Listing a preview of available PIBES commands =====\n\n>'help' or 'ls'     - displays this shell information\n>whoami             - display current username\n>time               - display current time\n>clear              - clear the display\n>(+)                - increase font size (scaled)\n>(-)                - decrease font size (scaled)\n>registersinfo      - print current register values\n>zerodiv            - testeo divide by zero exception\n>invopcode          - testeo invalid op code exception\n>whoami             - prints current username\n>memtest            - test memory manager\n>schetest           - test scheduler\n>priotest           - priority scheduler\n>runtestprocesses   - run test processes\n>testsync           - test synchro\n>ps                 - list all processes\n>cat                - cat file\n>loop               - prints short greeting and process PID\n>kill [PID]         - kill specified process\n>block [PID]        - block specified process\n>unblock [PID]      - unblock specified process\n>nice [PID] [prio]  - change a given's process priority\n>philo              - test philosophers\n>wc                 - counts the total amount of input lines\n>filter             - filt all input vocals\n>mem                - print memory state\n>exit               - exit PIBES OS\n";
	for(int i = 0; i < strlen(help); i++){
		write_char(help[i]);
	}
	write_string("\n    >testchildren                - test children processes\n", MAX_BUFF);
	return 0;
}

uint64_t cmd_undefined(uint64_t argc, char *argv[]){
	prints("\n\nbash: command not found: \"", MAX_BUFF);
	prints(command, MAX_BUFF);
	prints("\" Use 'help' or 'ls' to display available commands", MAX_BUFF);
	return 0;
}

uint64_t cmd_time(uint64_t argc, char *argv[]){
	getTime();
	return 0;
}

uint64_t cmd_exit(uint64_t argc, char *argv[]){
	prints("\n\nExiting PIBES OS\n", MAX_BUFF);
	terminate = 1;
	return 0;
}

uint64_t cmd_clear(uint64_t argc, char *argv[]){
	clear_scr();
	return 0;
}

uint64_t cmd_registersinfo(uint64_t argc, char *argv[]){
	registerInfo();
	return 0;
}

uint64_t cmd_invOpcode(uint64_t argc, char *argv[]){
	test_invopcode();
	return 0;
}

uint64_t cmd_zeroDiv(uint64_t argc, char *argv[]){
	test_zerodiv();
	return 0;
}

uint64_t cmd_charsizeplus(uint64_t argc, char *argv[]){
	cmd_clear(0, NULL);
	increaseScale();
	printPrompt();
	return 0;
}

uint64_t cmd_charsizeminus(uint64_t argc, char *argv[]){
	cmd_clear(0, NULL);
	decreaseScale();
	printPrompt();
	return 0;
}

void handleSpecialCommands(char c){
	if (c == PLUS){
		cmd_charsizeplus(0, NULL);
	} else if (c == MINUS){
		cmd_charsizeminus(0, NULL);
	} else if (isUpperArrow(c)){
		historyCaller(-1);
	} else if (isDownArrow(c)){
		historyCaller(1);
	} else {
		line[linePos++] = c;
		printc(c);
	}
}

uint64_t cmd_ps(uint64_t argc, char *argv[]){
	char *processes = sys_list_processes();
	write_string(processes, MAX_BUFF);
	sys_mem_free(processes);
	return 0;
}

uint64_t cmd_eliminator(uint64_t argc, char *argv[]){
	int numPlayers;
	if (parameter[0] == '\0')
	{
		numPlayers = 1;
	}
	else
	{
		numPlayers = atoi(parameter);
	}

	if (numPlayers == 1 || numPlayers == 2 || parameter[0] == '\0')
	{
		int playAgain = 1;
		while (playAgain)
		{
			playAgain = eliminator(numPlayers);
		}
	}
	else
	{
		prints("\nERROR: Invalid number of players. Only 1 or 2 players allowed.", MAX_BUFF);
	}
	return 0;
}

void historyCaller(int direction){

    for (int i = 0; i < linePos; i++){
        printc('\b');
    }
    for (int i = 0; i < linePos; i++){
        printc(' '); 
    }
    for (int i = 0; i < linePos; i++){
        printc('\b');
    }

    commandIterator += direction;

    if (commandIterator < 0){
        commandIterator = 0;
    } else if (commandIterator >= commandIdxMax){
        commandIterator = commandIdxMax - 1;
    }

    strcpy(line, commandHistory[commandIterator]);
    linePos = strlen(commandHistory[commandIterator]);

    prints(commandHistory[commandIterator], MAX_BUFF);
}



uint64_t cmd_ascii(uint64_t argc, char *argv[]){
	// int asciiIdx = random() % ASCII_ART_COUNT;
	// size_t splash_length = 0;
	
	// asciiIdx = asciiIdx % ASCII_ART_COUNT; 
	// while (splash_length < MAX_ASCII_HEIGHT && ascii[asciiIdx][splash_length] != NULL){
	// 	splash_length++;
	// }

	// for (int i = 0; i < splash_length; i++){
	// 	write_string(ascii[asciiIdx][i], MAX_BUFF);
	// 	write_char('\n');
	// }
	return 0;
}

uint64_t cmd_run_test_processes(uint64_t argc, char *argv[]){
	run_test_processes();
	return 0;
}

uint64_t cmd_test_sync(uint64_t argc, char *argv[]) {
    char *arg[] = {"5", "1", 0};
	uint64_t pid = create_process_foreground(0, &test_sync, 2, arg, 0, 0);	//Le paso 0 como fd_ids y fd_count, le pone stdin y stdout
	sys_wait_pid(pid);
	printsColor("CREATED 'test_sync' PROCESS!\n", MAX_BUFF, RED);
	return 0;
}

void newLineUsername(){
	int i;
	for (i = 0; i < USERNAME_SIZE-1 && line[i] != '\0'; i++) {
		username[i] = line[i];
	}
	username[i] = '\0';
	usernameLength = i;

	for (int i = 0; line[i] != '\0'; i++)
	{
		line[i] = 0;
		command[i] = 0;
		parameter[i] = 0;
	}
	linePos = 0;

	prints("\n", MAX_BUFF);
	clear_scr();
}

void welcome()
{
	char c;
	prints("\nPlease enter your username: ", MAX_BUFF);
	while (!strcmp(username, "user")){
		c = getChar();
		printLine(c, strcmp(username, "user"));
	}

	for (int i = 0; pibes[i] != NULL; i++){
		printsColor(pibes[i], MAX_BUFF, GREEN);
		printc('\n');
	}

	printsColor("\n    Welcome to PIBES OS, an efficient and simple operating system\n", MAX_BUFF, GREEN);
	printsColor("    Developed by the PIBES team\n", MAX_BUFF, GREEN);
	printsColor("    Here's a list of available commands\n", MAX_BUFF, GREEN);
	cmd_help(0, NULL);
}

uint64_t cmd_philo(uint64_t argc, char *argv[]){
	philosophers_simulation();
	return 0;
}

uint64_t cmd_wc(uint64_t argc, char *argv[]){
	int lines = 0;
	char c;
	while((c = sys_read_fd(0)) != -1) {
		write_char(c);
		if (c == '\n'){
			lines++;
		}
	}
	write_string("Total lines: ", strlen("Total lines: "));
	write_int(lines, MAX_BUFF);
	write_char('\n');
	return 0;
}

uint64_t cmd_filter(uint64_t argc, char *argv[]){
	int vowels = 0;
	char c;
	while((c = sys_read_fd(0)) != -1) {
		write_char(c);
		vowels += isVowel(c);
	}
	write_string("Total vowels: ", strlen("Total vowels: "));
	write_int(vowels, MAX_BUFF);
	return 0;
}


uint64_t cmd_block(uint64_t argc, char *argv[]){
	uint64_t pid = str_to_int(parameter);
	sys_block(pid);
	return 0;
}

uint64_t cmd_unblock(uint64_t argc, char *argv[]){
	uint64_t pid = str_to_int(parameter);
	sys_unblock(pid);
	return 0;
}

uint64_t cmd_nice(uint64_t argc, char *argv[]){
	// char * parameter2 = parameter;
	// char secondParameter[20];
	// int i, j=0;
	// for(i=0; parameter2[i++] != ' ';){}
	// for(; parameter2[i]; i++){
	// 	secondParameter[j++] = parameter2[i];
	// }
	uint64_t pid = str_to_int(parameter);
	// uint8_t priority = str_to_int(secondParameter);
	sys_nice(pid, 0);
	return 0;
}

uint64_t cmd_mem(uint64_t argc, char *argv[]){
	char *mem_state = sys_mem_state();
	write_string(mem_state, strlen(mem_state));
	sys_mem_free(mem_state);
	return 0;
}

uint64_t cmd_test_children(uint64_t argc, char *argv[]){
	create_process(0, (program_t)child0, 0, NULL, NULL, 0);
	return 1;
}

void child0(uint64_t argc, char *argv[]){
	create_process(0, (program_t)child1, 0, NULL, NULL, 0);
	// sys_wait(300);
	// write_string("Child 0 is dead\n", MAX_BUFF);
	while(1){
		__asm__("hlt");
	}
}

void child1(uint64_t argc, char *argv[]){
	create_process(0, (program_t)child2, 0, NULL, NULL, 0);
	// sys_wait(300);
	// write_string("Child 1 is dead\n", MAX_BUFF);
	while(1){
		__asm__("hlt");
	}
}

void child2(uint64_t argc, char *argv[]){
	sys_block(sys_getPID());
	// sys_wait(300);
	// write_string("Child 2 is dead\n", MAX_BUFF);
	while(1){
		__asm__("hlt");
	}
}
