#ifndef _PHILO_H_
#define _PHILO_H_

#include <stdint.h>

#define MAX_PHILOSOPHERS 16
#define MIN_PHILOSOPHERS 2

typedef enum { THINKING, HUNGRY, EATING } State;

typedef struct {
    int id;
    State state;
    uint64_t pid; // PID del proceso filósofo
} Philosopher;

#define MAX_PHILOSOPHERS 16
#define MIN_PHILOSOPHERS 2

#define LEFT(i) ((i + num_philosophers - 1) % num_philosophers)
#define RIGHT(i) ((i + 1) % num_philosophers)

#define THINKING 0
#define HUNGRY 1
#define EATING 2


uint64_t init_philosophers(uint64_t argc, char *argv[]);
void philosopher_function(uint64_t argc, char *argv[]);
void think(int id);
void eat(int id);
void take_forks(int id);
void put_forks(int id);
void test(int id);
int random_time();
void create_philosopher_process(int id);
void print_state();
void add_philosopher();
void remove_philosopher();
char get_non_blocking_char();


#endif