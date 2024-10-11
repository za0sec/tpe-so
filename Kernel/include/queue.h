#ifndef __queue
#define __queue
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct q_cdt * q_adt;

typedef struct pcb {
    uint64_t pid;
    uint64_t rsp;
    enum {
        READY,
        RUNNING,
        BLOCKED,
        TERMINATED
    } state;
} pcb_t;

void free_q(q_adt q);
void add(q_adt q, char * path, size_t size_mb);
q_adt new_q(int (*compare)(size_t, size_t));
void to_begin(q_adt q);
int has_next(q_adt q);
char * next(q_adt q);


#endif // __queue