#include <sys_calls.h>
#include <stdint.h>
#include <userlib.h>
#include <kitty.h>

int main(uint64_t argc, char *argv[]) {
	welcome();

	kitty();

	return 0;
} 