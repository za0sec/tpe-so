// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
    // No alignment
#include <memManager.h>
#include <utils.h>

void * start;
int size, current;
void * free_ptrs[CHUNK_COUNT];

void mem_init(void *ptr, int s){
    start = (void *)ALIGN_POINTER(ptr, WORD_ALIGN);
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

char *mem_state(){
    char *buf = mem_alloc(256);  
    if (buf == NULL) {
        return NULL;  
    }
    int offset = 0;

    strcpy(buf + offset, "Memory State:\n", strlen("Memory State:\n"));
    offset += strlen("Memory State:\n");

    strcpy(buf + offset, "Total chunks: ", strlen("Total chunks: "));
    offset += strlen("Total chunks: ");
    intToStr(CHUNK_COUNT, buf + offset);
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    strcpy(buf + offset, "Chunk size: ", strlen("Chunk size: "));
    offset += strlen("Chunk size: ");
    intToStr(CHUNK_SIZE, buf + offset);
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    strcpy(buf + offset, "Current index: ", strlen("Current index: "));
    offset += strlen("Current index: ");
    intToStr(current, buf + offset);
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    int free_chunks = CHUNK_COUNT - current;
    strcpy(buf + offset, "Free chunks: ", strlen("Free chunks: "));
    offset += strlen("Free chunks: ");
    intToStr(free_chunks, buf + offset);
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    buf[offset] = '\0';  
    return buf; 
}
