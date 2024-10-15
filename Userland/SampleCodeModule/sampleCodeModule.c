#include <sys_calls.h>
#include <stdint.h>
#include <userlib.h>
#include <kitty.h>
#include <tests.h>

int main(uint64_t argc, char *argv[]) {
	sys_create_process(5, &kitty, 0, argv);

	return 0;
}