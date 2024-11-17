#ifndef philo_h
#define philo_h

#include <stdint.h>

typedef struct philo {
    uint64_t pid;
    char *sem_name;
    int state;
    int hunger;
} philo;

#define MUTEX_PHILOSOPHERS "philo_mutex"
#define MUTEX_COUNT_PHILO "count_philo_mutex"
#define MAX_PHILOS 10
#define MIN_PHILOS 3
#define INITIAL_PHILO 5

void philosophers_simulation();
void add_philosopher(int philo);
void print_state();
void philosopher(int argc, char *argv[]);
void think(int philo);
void eat(int philo);

#endif