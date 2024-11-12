// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdint.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <userlib.h>
#include <philo.h>

static Phylo philosophers[MAX_PHYLOS];

static int thinkers;    

void sleep(uint64_t ms) {
    sys_wait(ms);
}

void init_philosophers(uint64_t argc, char *argv[]) {
    // Inicializando semáforos
    sys_sem_open(MUTEX_ARRAY, 1);
    sys_sem_open(MUTEX_THINKERS, 1);

    // Crear proceso controlador
    sys_create_process_foreground(0, &controllers_handler, 0, NULL);

    thinkers = INITIAL_THINKERS;

    // Inicializar semáforos para los filósofos y crear procesos
    for (int i = 0; i < MAX_PHYLOS; i++)
        add_philosopher(i);

    // Esperar a que terminen los filósofos
    for (int i = 0; i < MAX_PHYLOS; i++)
        sys_wait_pid(philosophers[i].pid);

    // Cerrar semáforos
    sys_sem_close(MUTEX_ARRAY);
    sys_sem_close(MUTEX_THINKERS);
}

void phyloProcess(uint64_t argc, char *argv[]) {
    int id = atoi(argv[0]);
    int left = id;
    int right = (id + 1) % MAX_PHYLOS;

    while (thinkers != 0) {
        if (id % 2) {
            sys_sem_wait(philosophers[right].sem_name);
            sys_sem_wait(philosophers[left].sem_name);
        } else {
            sys_sem_wait(philosophers[left].sem_name);
            sys_sem_wait(philosophers[right].sem_name);
        }

        eat(id);

        sys_sem_post(philosophers[left].sem_name);
        sys_sem_post(philosophers[right].sem_name);

        think(id);
    }

    sys_kill(sys_getPID()); // Finalizar proceso
}

void think(int phy) {
    sys_sem_wait(MUTEX_ARRAY);
    philosophers[phy].state = 0;
    reprint();
    sys_sem_post(MUTEX_ARRAY);
    // sleep(GetUniform(phy));
}

void eat(int phy) {
    sys_sem_wait(MUTEX_ARRAY);
    philosophers[phy].state = 1;
    reprint();
    sys_sem_post(MUTEX_ARRAY);
    // sleep(GetUniform(phy));
}

void add_philosopher(int phy) {
    char indexStr[20];
    intToStr(phy, indexStr);

    // Crear semáforo para el filósofo
    sys_sem_open(indexStr, 1);
    strcpy(philosophers[phy].sem_name, indexStr);

    // Crear proceso filósofo
    char *philo_argv[] = { indexStr, NULL };
    philosophers[phy].pid = sys_create_process_foreground(0, (program_t)phyloProcess, 1, philo_argv);
}

uint64_t controllers_handler(uint64_t argc, char *argv[]) {
    while (1) {
        char aux = sys_read(0);

        switch (aux) {
            case 'a':
                sys_sem_wait(MUTEX_THINKERS);
                if(thinkers < MAX_PHYLOS)
                    thinkers++;
                sys_sem_post(MUTEX_THINKERS);
                break;
            case 'r':
                sys_sem_wait(MUTEX_THINKERS);
                if(thinkers > MIN_PHYLOS)
                    thinkers--;
                sys_sem_post(MUTEX_THINKERS);
                break;
            case 'x':
                thinkers = 0;
                return 0;
        }
    }
    return 0;
}

void reprint() {
    if(thinkers == 0)
        return;
    sys_sem_wait(MUTEX_THINKERS);
    
    // char buffer[MAX_BUFF];
    // sprintf(buffer, "%d ->\t ", thinkers);
    // write_string(buffer, MAX_BUFF);
    
    for (int i = 0; i < thinkers; i++) {
        if(philosophers[i].state == 1) {
            write_string("E   ", MAX_BUFF);
        } else {
            write_string(".   ", MAX_BUFF);
        }
    }
    write_string("\n\n", MAX_BUFF);
    sys_sem_post(MUTEX_THINKERS);
}
