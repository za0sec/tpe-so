#include <userlib.h>
#include <test_util.h>
#include <sys_calls.h>
#include <tests.h>

#define MAX_BLOCKS 128
#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 2 // TODO: Change as required


enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

uint64_t test_mm(uint64_t argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = sys_mem_alloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_mem_init(mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_mem_free(mm_rqs[i].address);
  }
}

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1)
    return -1;

  if ((max_processes = satoi(argv[0])) <= 0)
    return -1;

  p_rq p_rqs[max_processes];

  while (1) {
    prints("======================= Starting test=======================\n", MAX_BUFF);
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      // p_rqs[rq].pid = sys_create_process("endless_loop", 0, 0, argvAux);
      p_rqs[rq].pid = sys_create_process(5, &endless_loop, 0, argvAux);

      if (p_rqs[rq].pid == -1) {
        prints("test_processes: ERROR creating process\n", MAX_BUFF);
        return -1;
      } else {
        prints("[test_processes] Process created with pid\n", MAX_BUFF);
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    char buffer[4096];
    sys_list_processes(buffer);
    prints(buffer, 4096);

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (sys_kill(p_rqs[rq].pid) == -1) {
                prints("test_processes: ERROR killing process\n", MAX_BUFF);
                return -1;
              }
              prints("[test_processes] Process killed\n", MAX_BUFF);
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (sys_block(p_rqs[rq].pid) == -1) {
                prints("test_processes: ERROR blocking process\n", MAX_BUFF);
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
              prints("[test_processes] Process blocked\n", MAX_BUFF);
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if ((p_rqs[rq].pid) == -1) {
            prints("test_processes: ERROR unblocking process\n", MAX_BUFF);
            return -1;
          }
          prints("[test_processes] Process unblocked\n", MAX_BUFF);
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}

void test_process(int n){
  while(1){
    sys_clear();
    switch(n){
      case 0:
        prints("++++++++++++++++++++++++++++++\n", MAX_BUFF);
      case 1:
        prints("------------------------------\n", MAX_BUFF);
      case 2:
        prints("******************************\n", MAX_BUFF);
      case 3:
        prints("//////////////////////////////\n", MAX_BUFF);
      case 4:
        prints(":::::::::::::::::::::::::::::::\n", MAX_BUFF);
      case 5:
        prints("0000000000000000000000000000000\n", MAX_BUFF);
      default:
        prints("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", MAX_BUFF);
    }
  }
}

int64_t test_scheduler_processes() {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes = 5;

  p_rq p_rqs[max_processes];

  prints("======================= CREATING TEST PROCESSES!=======================\n", MAX_BUFF);
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      // p_rqs[rq].pid = sys_create_process("endless_loop", 0, 0, argvAux);
      char *argv[] = {rq};
      p_rqs[rq].pid = sys_create_process(5, &test_process, 0, argv);

      if (p_rqs[rq].pid == -1) {
        prints("ERROR creating process\n", MAX_BUFF);
        return -1;
      } else {
        prints("Process created with pid\n", MAX_BUFF);
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    char buffer[4096];
    sys_list_processes(buffer);
    prints(buffer, 4096);
}

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = sys_create_process(5, &endless_loop, 0, argv);

  bussy_wait(WAIT);
  prints("\nCHANGING PRIORITIES...\n", MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    //my_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  prints("\nBLOCKING...\n" ,MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  prints("CHANGING PRIORITIES WHILE BLOCKED...\n",MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    //my_nice(pids[i], MEDIUM);

  prints("UNBLOCKING...\n",MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_unblock(pids[i]);

  bussy_wait(WAIT);
  prints("\nKILLING...\n",MAX_BUFF);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_kill(pids[i]);
}