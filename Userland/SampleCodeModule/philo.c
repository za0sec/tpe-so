#include <stdint.h>
#include <sys_calls.h>
#include <stdio.h>
#include <stdlib.h>
#include <userlib.h>
#include <philo.h>

Philosopher philosophers[MAX_PHILOSOPHERS];
int num_philosophers = 5; // Número inicial de filósofos
void *mutex; // Semáforo para sección crítica
void *semaphores[MAX_PHILOSOPHERS]; // Semáforos para cada filósofo

uint64_t init_philosophers(uint64_t argc, char *argv[]) {
    // Inicialización de semáforos y filósofos
    mutex = sys_sem_open("mutex", 1);
    if (mutex == NULL) {
        write_string("Failed to initialize mutex semaphore.\n", MAX_BUFF);
        return -1;
    }

    for (int i = 0; i < num_philosophers; i++) {
        char sem_name[20];
        intToStr(i, sem_name);
        semaphores[i] = sys_sem_open(sem_name, 0);
        if (semaphores[i] == NULL) {
            write_string("Failed to initialize semaphore for philosopher ", MAX_BUFF);
            printDec(i);
            write_string("\n", 1);
            return -1;
        }

        philosophers[i].id = i;
        philosophers[i].state = THINKING;

        create_philosopher_process(i);
    }

    write_string("Press 'a' to add a philosopher, 'r' to remove, 'q' to quit.\n", MAX_BUFF);

    // Bucle principal
    int running = 1;
    while (running) {
        char input = get_non_blocking_char();

        if (input == 'a') {
            add_philosopher();
        } else if (input == 'r') {
            remove_philosopher();
        } else if (input == 'q') {
            running = 0;
        }

        print_state();
        while (sys_getSeconds() % 2 != 0);
    }

    // Limpieza
    for (int i = 0; i < num_philosophers; i++) {
        sys_kill(philosophers[i].pid);
        char sem_name[20];
        intToStr(i, sem_name);
        sys_sem_close(semaphores[i]);
    }
    sys_sem_close(mutex);

    return 0;
}

void philosopher_function(uint64_t argc, char *argv[]) {
    int id = atoi(argv[0]);

    while (1) {
        think(id); // pensar();
        take_forks(id); // tomar_tenedores(i);
        eat(id); // comer();
        put_forks(id); // poner_tenedores(i);
    }
}

void think(int id) {
    // Simular pensando
    while (sys_getSeconds() % 2 != 0);
}

void eat(int id) {
    // Simular comiendo
    while (sys_getSeconds() % 2 != 0);
}

void take_forks(int id) {
    sys_sem_wait(mutex); // down(&mutex);
    philosophers[id].state = HUNGRY; // estado[i] = HAMBRIENTO;
    test(id); // probar(i);
    sys_sem_post(mutex); // up(&mutex);
    sys_sem_wait(semaphores[id]); // down(&s[i]);
}

void put_forks(int id) {
    sys_sem_wait(mutex); // down(&mutex);
    philosophers[id].state = THINKING; // estado[i] = PENSANDO;
    test(LEFT(id)); // probar(IZQUIERDO);
    test(RIGHT(id)); // probar(DERECHO);
    sys_sem_post(mutex); // up(&mutex);
}

void test(int id) {
    if (philosophers[id].state == HUNGRY &&
        philosophers[LEFT(id)].state != EATING &&
        philosophers[RIGHT(id)].state != EATING) {
        philosophers[id].state = EATING; // estado[i] = COMIENDO;
        sys_sem_post(semaphores[id]); // up(&s[i]);
    }
}

int random_time() {
    return 500 + (sys_getPID() % 500); // Retorna un tiempo entre 500 y 999 ms
}

void create_philosopher_process(int id) {
    char buff[10];
    intToStr(id, buff);
    char *argv[] = {buff, NULL};
    uint64_t pid = sys_create_process(1, philosopher_function, 1, argv);
    philosophers[id].pid = pid;
}

void print_state() {
    sys_sem_wait(mutex);
    for (int i = 0; i < num_philosophers; i++) {
        if (philosophers[i].state == EATING) {
            write_char('E');
        } else {
            write_char('.');
        }
    }
    write_char('\n');
    sys_sem_post(mutex);
}

void add_philosopher() {
    if (num_philosophers >= MAX_PHILOSOPHERS) {
        write_string("Maximum number of philosophers reached.\n", MAX_BUFF);
        return;
    }

    sys_sem_wait(mutex);

    int id = num_philosophers;
    num_philosophers++;

    char sem_name[20];
    intToStr(id, sem_name);
    semaphores[id] = sys_sem_open(sem_name, 0);

    philosophers[id].id = id;
    philosophers[id].state = THINKING;

    create_philosopher_process(id);

    sys_sem_post(mutex);
}

void remove_philosopher() {
    if (num_philosophers <= MIN_PHILOSOPHERS) {
        write_string("Minimum number of philosophers reached.\n", MAX_BUFF);
        return;
    }

    sys_sem_wait(mutex);

    num_philosophers--;
    int id = num_philosophers;

    sys_kill(philosophers[id].pid);
    sys_sem_close(semaphores[id]);

    sys_sem_post(mutex);
}

char get_non_blocking_char() {
    char c;
    if ((c = sys_read(0)) > 0) {
        return c;
    } else {
        return 0;
    }
}
