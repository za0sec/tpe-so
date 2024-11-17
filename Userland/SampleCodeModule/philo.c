// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <userlib.h>
#include <sys_calls.h>
#include <philo.h>

philo philosophers[MAX_PHILOS];
int count_philo;

void philosophers_simulation() {
    char buf[MAX_BUFF] = {0};
    count_philo = INITIAL_PHILO;

    write_string("PHILOSOPHER SIMULATION STARTED\nPress 'a' to add a philosopher, 'r' to remove one or 'x' to exit\n", MAX_BUFF);
    wait(99);

    if( sys_sem_open(MUTEX_PHILOSOPHERS, 1) != 0 || sys_sem_open(MUTEX_COUNT_PHILO, 1) != 0 ) {
        return;
    }

    for (int i = 0; i < MAX_PHILOS; i++){
        add_philosopher(i);
    } 

    char c = getChar();
    while (c != 'x') {
        if(c == 'a'){
            sys_sem_wait(MUTEX_COUNT_PHILO);

            if(count_philo < MAX_PHILOS){
                count_philo++;
                write_string("Added philosopher\n", MAX_BUFF);
            }

            sys_sem_post(MUTEX_COUNT_PHILO);
        } else if (c == 'r'){
            sys_sem_wait(MUTEX_COUNT_PHILO);

            if(count_philo > MIN_PHILOS){
                count_philo--;
                write_string("Removed philosopher\n", MAX_BUFF);
            }

            sys_sem_post(MUTEX_COUNT_PHILO);
        }
        c = getChar();
    }

    for (int i = 0; i < MAX_PHILOS; i++){
        if(philosophers[i].pid == 0){
            continue;
        }

        sys_kill(philosophers[i].pid);
        sys_mem_free(philosophers[i].sem_name);
        sys_sem_close(philosophers[i].sem_name);
        philosophers[i].sem_name = NULL;
        philosophers[i].pid = 0;
    }

    for (int i = 0; i < count_philo; i++){
        write_string("Philosopher ", MAX_BUFF);
        intToStr(i, buf);
        write_string(buf, MAX_BUFF);
        write_string(" ate ", MAX_BUFF);
        intToStr(philosophers[i].hunger, buf);
        write_string(buf, MAX_BUFF);
        write_string(" times\n", MAX_BUFF);
    }

    sys_sem_close(MUTEX_PHILOSOPHERS);
    sys_sem_close(MUTEX_COUNT_PHILO);

    write_string("PHILOSOPHER SIMULATION FINISHED\n", MAX_BUFF);
}

void add_philosopher(int philo) {
    char * indexStr = sys_mem_alloc(sizeof(char) * 2);
    intToStr(philo, indexStr);
    sys_sem_open(indexStr, 1);
    philosophers[philo].sem_name = indexStr;

    philosophers[philo].hunger = 0;

    char *philo_argv[2] = {indexStr, NULL};
    philosophers[philo].pid = create_process(0, (program_t)philosopher, 1, mem_alloc_args(philo_argv), NULL, 0);
}

void philosopher(int argc, char *argv[]) {
    int id = satoi(argv[0]);                  
    int left = id;
    int right = (id + 1) % MAX_PHILOS;

    while (count_philo != 0) {
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
}

void think(int philo) {
    sys_sem_wait(MUTEX_PHILOSOPHERS);
    philosophers[philo].state = 0;
    print_state();
    sys_sem_post(MUTEX_PHILOSOPHERS);

    sys_wait(GetUniform(philo));
}

void eat(int philo) {
    sys_sem_wait(MUTEX_PHILOSOPHERS);
    philosophers[philo].state = 1; 

    if(philo < count_philo){
        philosophers[philo].hunger++;
    }
    
    print_state();
    sys_sem_post(MUTEX_PHILOSOPHERS);

    sys_wait(GetUniform(philo));
}

void print_state() {
    char *state_buf;

    if(count_philo == 0){
        return;
    }

    sys_sem_wait(MUTEX_COUNT_PHILO);
 
    for (int i = 0; i < count_philo; i++){
        state_buf = philosophers[i].state == 1 ? "E   " : ".   "; 
        write_string(state_buf, MAX_BUFF);
    }
    write_string("\n", MAX_BUFF);
    
    sys_sem_post(MUTEX_COUNT_PHILO);
}
