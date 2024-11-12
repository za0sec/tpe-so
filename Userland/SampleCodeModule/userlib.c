#include <sys_calls.h>
#include <stdio.h>
#include <exc_test.h>
#include <time.h>
#include <userlib.h>
#include <test_util.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

const Color BLACK = {0, 0, 0};
const Color WHITE = {255, 255, 255};
const Color RED = {0, 0, 255};
const Color LIGHT_BLUE = {173, 216, 230};
const Color BLUE = {255, 0, 0};
const Color ORANGE = {16, 160, 255};
const Color YELLOW = {30, 224, 255};
const Color PURPLE = {255, 32, 160};
const Color PINK = {100, 0, 244};
const Color GREEN = {0, 255, 0};
const Color LIGHT_RED = {0, 255, 255};
const Color LIGHT_PURPLE = {255, 0, 255};
const Color LIGHT_ORANGE = {0, 160, 255};
const Color LIGHT_YELLOW = {0, 224, 255};
const Color LIGHT_PINK = {0, 100, 244};
const Color LIGHT_GREEN = {0, 255, 0};

static char buffer[64] = {'0'};

int scr_height;
int scr_width;

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

uint64_t create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[10], uint64_t fd_count){
	int *fd_ids_array;
	if(fd_ids == NULL){
		fd_ids_array = NULL;
		fd_count = 0;
	} else {
		fd_ids_array = sys_mem_alloc(sizeof(int) * 10);
		for(int i = 0; i < fd_count; i++){
			fd_ids_array[i] = fd_ids[i];
		}
	}

	

	sys_create_process_set_fd(fd_ids_array, fd_count);

	uint64_t pid = sys_create_process_foreground(priority, program, argc, argv);

	sys_mem_free(fd_ids_array);

	return pid;
}

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[10], uint64_t fd_count){
	int *fd_ids_array;
	if(fd_ids == NULL){
		fd_ids_array = NULL;
		fd_count = 0;
	} else {
		fd_ids_array = sys_mem_alloc(sizeof(int) * 10);
		for(int i = 0; i < fd_count; i++){
			fd_ids_array[i] = fd_ids[i];
		}
	}

	sys_create_process_set_fd(fd_ids_array, fd_count);

	uint64_t pid = sys_create_process(priority, program, argc, argv);

	sys_mem_free(fd_ids_array);

	return pid;
}

void triggerSpeaker(uint32_t frequence, uint64_t duration)
{
	sys_playSpeaker(frequence, duration);
}

void playMelody(NoteType *melody, int length)
{
	for (int i = 0; i < length; i++)
	{
		triggerSpeaker(melody[i].tone, melody[i].duration);
	}
}

void write_char(char c){
	sys_write_fd(STDOUT, c);
}

void printc(char c)
{
	sys_write(STDOUT, c);
}

void printcColor(char c, Color color)
{
	sys_writeColor(STDOUT, c, color);
}

void drawCursor()
{
	while (1){
		sys_drawCursor();
	}
}

void prints(const char *str, int lenght)
{
	for (int i = 0; i < lenght && str[i] != 0; i++)
	{
		printc(str[i]);
	}
}

void write_string(const char *str, int lenght)
{
	for (int i = 0; i < lenght && str[i] != 0; i++)
	{
		write_char(str[i]);
	}
}

void write_int(int integer, int length){
	char myBuff[20];
	intToStr(integer, myBuff);
	write_string(myBuff, length);
}

void printsColor(const char *str, int lenght, Color color)
{
	for (int i = 0; i < lenght && str[i] != 0; i++)
	{
		printcColor(str[i], color);
	}
}

char getChar()
{
	return (char)sys_read(0);
}

int isChar(char c)
{
	if ((c >= '!' && c <= '}'))
	{
		return 1;
	}
	return 0;
}

int isUpperArrow(char c)
{
	return c == 0x48;
}

int isDownArrow(char c)
{
	return c == 0x50;
}

int isDigit(char c)
{
	if (c >= '0' && c <= '9')
	{
		return 1;
	}
	return 0;
}

int strlen(const char *str)
{
	int i = 0;
	while (str[i] != 0)
	{
		i++;
	}

	return i;
}

char *strcpy(char *dest, const char *src)
{
	int i = 0;
	while (src[i] != 0)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return dest;
}

char *strcpyForParam(char *dest, const char *src, const char *src2)
{
	int i = 0, k = 0;
	while (src[i] != 0)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = ' ';
	i++;
	while (src2[k] != 0)
	{
		dest[i] = src2[k];
		i++;
		k++;
	}
	dest[i] = 0;
	return dest;
}

int strcmp(const char *str1, const char *str2)
{
	int i = 0;
	int ret = 0;

	while (str1[i] != 0 && str2[i] != 0)
	{
		if (str1[i] - str2[i] != 0)
		{
			return str1[i] - str2[i];
		}
		i++;
	}

	if (str1[i] != 0 || str2[i] != 0)
	{
		return str1[i] - str2[i];
	}

	return ret;
}

int random()
{
	return getSeconds() % 3;
}

uint64_t str_to_int(char *str){
	return charToInt(str);
}

uint64_t charToInt(char *str)
{
	uint64_t ret = 0;

	if (!isDigit(str[0]))
	{
		ret += str[0] - '0';
		return -1;
	}

	for (int i = 0; str[i] != 0; i++)
	{
		if (!isDigit(str[i]))
		{
			return -1;
		}
		ret *= 10;
		ret += str[i] - '0';
	}
	return ret;
}

void printBase(uint64_t value, uint32_t base)
{
	uintToBase(value, buffer, base);
	for (int i = 0; buffer[i] != '\0'; i++)
	{
		printc(buffer[i]);
	}
}

void printDec(uint64_t value)
{
	printBase(value, (uint32_t)10);
}

void printHex(uint64_t value)
{
	printBase(value, (uint32_t)16);
}

void printBin(uint64_t value)
{
	printBase(value, (uint32_t)2);
}

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
	return digits;
}

void clear_scr()
{
	sys_clear();
}

int get_scrWidth()
{
	return scr_width;
}

int get_scrHeight()
{
	return scr_height;
}

static void set_screSize()
{
	scr_width = sys_scrWidth();
	scr_height = sys_scrHeight();
}

void drawRectangle(int x, int y, int x2, int y2, Color color)
{
	set_screSize();
	if ((x >= 0 && x + x2 < scr_width) && ((y >= 0 && y + y2 < scr_height)))
	{
		sys_drawRectangle(x, y, x2, y2, color);
	}
}

void wait(uint64_t ms){
	sys_wait(ms);
}

static const char *register_names[17] = {
	"RIP", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"};

static void uint64ToHex(uint64_t n, char buf[16])
{
	int i = 15;
	do
	{
		int digit = n % 16;
		buf[i] = (digit < 10 ? '0' : ('A' - 10)) + digit;
		n /= 16;
	} while (i-- != 0);
}

void registerInfo()
{
	char hexbuf[19];
	hexbuf[0] = '0';
	hexbuf[1] = 'x';
	hexbuf[18] = '\0';

	uint64_t registers[17];
	int i = sys_registerInfo(registers);

	printc('\n');
	if (i == 1)
	{
		for (int i = 0; i < 17; i++)
		{
			prints(register_names[i], 100);
			prints(": ", 100);
			uint64ToHex(registers[i], hexbuf + 2);
			prints(hexbuf, 100);
			if (i % 4 == 3)
				printc('\n');
			else
				prints("   ", 100);
		}
	}
	else
	{
		prints("\nERROR: You must take a snapshot first. Press 'Shift' and 'S' at any time to take a snapshot.\n", 100);
	}
}

void test_invopcode()
{
	ex_invopcode();
}

void test_zerodiv()
{
	ex_zerodiv();
}

void increaseScale()
{
	sys_pixelPlus();
}

void decreaseScale()
{
	sys_pixelMinus();
}

int atoi(const char *str){
	int result = 0;
	int sign = 1;
	int i = 0;

	// Handle leading whitespace
	while (str[i] == ' ')
	{
		i++;
	}

	// Handle sign
	if (str[i] == '-' || str[i] == '+')
	{
		sign = (str[i++] == '-') ? -1 : 1;
	}

	// Convert digits to integer
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i++] - '0');
	}

	return sign * result;
}

int print_mem(uint64_t mem){
	return sys_printmem(mem);
}

void cat(){
	char c;
	while((c = sys_read_fd(0)) != -1) {
		write_char(c);
	}
}

// Convierte un entero a cadena
void intToStr(int value, char *str)
{
    int index = 0;
    int isNegative = 0;

    if (value == 0)
    {
        str[index++] = '0';
        str[index] = '\0';
        return;
    }

    if (value < 0)
    {
        isNegative = 1;
        value = -value;
    }

    while (value > 0)
    {
        str[index++] = (value % 10) + '0';
        value /= 10;
    }

    if (isNegative)
    {
        str[index++] = '-';
    }

    str[index] = '\0';

    for (int i = 0; i < index / 2; i++)
    {
        char temp = str[i];
        str[i] = str[index - i - 1];
        str[index - i - 1] = temp;
    }
}

char *memcpy(char *dest, const char *src, uint64_t size){
	int i = 0;
	while (i < size && src[i] != 0)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return dest;
}

int stringArrayLen(char **array) {
	int len = 0;
	while (*(array++) != NULL)
		len++;
	return len;
}

char **mem_alloc_args(char **args) {
  int argc = stringArrayLen(args), totalArgsLen = 0;
  int argsLen[argc];
  
  for (int i = 0; i < argc; i++) {
    argsLen[i] = strlen(args[i]) + 1;
    totalArgsLen += argsLen[i];
  }
  
  char **newArgsArray = (char **) sys_mem_alloc(sizeof(char *) * (argc + 1) + totalArgsLen);
  
  char *charPosition = (char *) (newArgsArray + argc + 1);
  
  for (int i = 0; i < argc; i++) {
    newArgsArray[i] = charPosition; 
    memcpy(charPosition, args[i], argsLen[i]);
    charPosition += argsLen[i];
  }
  
  newArgsArray[argc] = NULL;
  return newArgsArray;
}

int isVowel(char lett){
    if(lett == 'a' || lett == 'e' || lett == 'i' || lett == 'o' || lett == 'u' || lett == 'A' || lett == 'E' || lett == 'I' || lett == 'O' || lett == 'U'){
        return 1;
    }
    return 0;
}