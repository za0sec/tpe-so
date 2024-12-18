#ifndef _MEM_MANAGER_H_
#define _MEM_MANAGER_H_

#include <stddef.h>
#include <stdint.h>

#define CHUNK_SIZE 4096
#define CHUNK_COUNT 32768
#define MEM_START 0xF00000
#define WORD_ALIGN 8
#define ALIGN_POINTER(ptr, alignment) \
    ((uintptr_t)(ptr) + ((alignment) - ((uintptr_t)(ptr) % (alignment))))

void mem_init(void *ptr, uint32_t s);

void *mem_alloc(uint32_t s);

void mem_free(void *ptr);

char *mem_state();

#endif // _MEM_MANAGER_H_
