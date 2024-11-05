#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <stdint.h>

#define SEMAPHORE_NAME_SIZE 32

typedef struct {
    lock_t sem_lock;
    char name[SEMAPHORE_NAME_SIZE];    
    int value;           // Valor del semáforo
    q_adt blocked_queue; // Cola de procesos bloqueados
} sem_t;

#define MAX_SEMAPHORES (CHUNK_SIZE / sizeof(sem_t))

int64_t sem_open(char *sem_id, uint64_t initialValue);
int64_t sem_close(char * sem_id);
void sem_wait(char * sem_name);
int64_t sem_post(char *sem_id);

#endif