#ifndef _MEM_MANAGER_H_
#define _MEM_MANAGER_H_

#include <stddef.h>
#include <stdint.h>

#define CHUNK_SIZE 16
#define CHUNK_COUNT 4096
#define MEM_START 0x300000
#define WORD_ALIGN 8
#define ALIGN_POINTER(ptr, alignment) \
    ((uintptr_t)(ptr) + ((alignment) - ((uintptr_t)(ptr) % (alignment))))

void *mem_init(int s);

void *mem_alloc(uint32_t s);

void mem_free(void *ptr);

void fill_stack(uint64_t sp);

#endif // _MEM_MANAGER_H_
