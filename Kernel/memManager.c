// No alignment
#include <memManager.h>

void * start;
int size, current;
void * free_ptrs[CHUNK_COUNT];

void *mem_init(void *p, int s){
    start = p;
    size = s;
    current = 0;
    for(int i = 0; i < CHUNK_COUNT; i++){
        free_ptrs[i] = start + i * CHUNK_SIZE;
    }
}

void *mem_alloc(uint32_t s){
    if(current >= CHUNK_COUNT || s > size){
        return NULL;
    }
    return free_ptrs[current++];
}

void mem_free(void *ptr){
    if(ptr < start || ptr > start + size){
        return;
    }
    
    free_ptrs[--current] = ptr;
}