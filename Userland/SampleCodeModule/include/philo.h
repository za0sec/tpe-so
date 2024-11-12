#define MAX_PHYLOS 10
#define MIN_PHYLOS 3
#define INITIAL_THINKERS 5

typedef struct phylo {
    uint64_t pid;
    char sem_name[20]; // Nombre del semáforo
    int state; // Estado del filósofo: 0 = pensando, 1 = comiendo
} Phylo;

#define MUTEX_ARRAY "access_array"

#define MUTEX_THINKERS "thinkers_mutex"

void init_philosophers(uint64_t argc, char *argv[]);
uint64_t controllers_handler(uint64_t argc, char *argv[]);
void reprint();
void eat(int phy);
void think(int phy);
void add_philosopher(int phy);
void phyloProcess(uint64_t argc, char *argv[]);