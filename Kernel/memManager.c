// No alignment
#include <memManager.h>

void * start;
int size, current;
void * free_ptrs[CHUNK_COUNT];

void *mem_init(int s){
    start = ALIGN_POINTER((void*)MEM_START, WORD_ALIGN);
    size = s;
    current = 0;
    for(int i = 0; i < CHUNK_COUNT; i++){
        free_ptrs[i] = start + i * CHUNK_SIZE;
    }
    return start;
}

void *mem_alloc(uint32_t s) {
    // Verificar si se excede el número de bloques o si el tamaño solicitado es mayor que el tamaño de un bloque
    if (current >= CHUNK_COUNT || s > CHUNK_SIZE) {
        return NULL;
    }

    // Alinear la dirección actual del bloque
    void *aligned_ptr = (void *)ALIGN_POINTER(free_ptrs[current], WORD_ALIGN);

    // Verificar si después de la alineación el bloque tiene suficiente espacio para 's' bytes
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