// No alignment
#include <memManager.h>

void * start;
int size, current;
void * free_ptrs[CHUNK_COUNT];

void mem_init(void *ptr, int s){
    start = ALIGN_POINTER(ptr, WORD_ALIGN);
    size = s;
    current = 0;
    for(int i = 0; i < CHUNK_COUNT; i++){
        free_ptrs[i] = start + i * CHUNK_SIZE;
    }
}

void *mem_alloc(uint32_t s) {
    if (current >= CHUNK_COUNT || s > CHUNK_SIZE) {
        return NULL;
    }

    void *aligned_ptr = (void *)ALIGN_POINTER(free_ptrs[current], WORD_ALIGN);

    if ((uintptr_t)aligned_ptr + s > (uintptr_t)free_ptrs[current] + CHUNK_SIZE) {
        current++; 
        if (current >= CHUNK_COUNT) {
            return NULL;  
        }
        aligned_ptr = (void *)ALIGN_POINTER(free_ptrs[current], WORD_ALIGN);
    }

    current++;
    return aligned_ptr;
}

void mem_free(void *ptr){
    if(ptr < start || ptr > start + size){
        return;
    }
    
    free_ptrs[--current] = ptr;
}