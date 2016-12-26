#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include "stats.h"
#include <semaphore.h>
#include <signal.h>
#define SEC_ACC 1000000000L
#define MAX_CLIENT 16

stats_t * stats_ptr;
key_t g_key = 0;

int myAtoi(char *str, int * res) {
  *res = 0;
  int loop = 0;
  while (str[loop]) {
    if (str[loop] >= '0' && str[loop] <= '9')
      *res = *res * 10 + str[loop] - '0';
    else
      return 0;
    loop++;
  }
  return 1;
}

void sighandler(int signo) {
  if (DEBUG)
  printf("sighandler function\n");
  if (SIGINT == signo) {
    if (stats_unlink(g_key)) {
      exit(1);
    }
    exit(0);
  }
}

int
main(int argc, char *argv[]) {
  int shmid;
  key_t key;
  char *shm, *s;
  int result_cl_opt_parsing = 0;
  stats_t process_stats;
  struct timespec tv, start, temp, end;
  struct sigaction action;
  double total_cputime;
  int int_key, priority, sleeptime_ns, cputime_ns, temp_cputime;
  // default value of arguments
  stats_ptr = NULL;
  g_key = key = 100, priority = 10, sleeptime_ns = 1000, cputime_ns = 1000000;
  while ((result_cl_opt_parsing = getopt(argc, argv,
  "k:p:s:c:")) != -1) {
    switch (result_cl_opt_parsing) {
    case 'k':
      myAtoi(optarg, &int_key);
      g_key = key = int_key;
      break;
    case 'p':
      myAtoi(optarg, &priority);
      break;
    case 's':
      myAtoi(optarg, &sleeptime_ns);
      break;
    case 'c':
      myAtoi(optarg, &cputime_ns);
      break;
    default :
      exit(1);
    }
  }
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = sighandler;
  sigaction(SIGINT, &action, NULL);
  // printf("Making call to stats_init\n");
  if ((shm = stats_init(key)) == NULL) {
    if (DEBUG) perror("shm_init");
    exit(0);
  }
  if (DEBUG) printf("returned from call to stats_init\n");
  if (DEBUG)
  printf( "Shared memory attached %p, size %d\n", shm, sizeof(shm));
  process_stats.counter = 0;
  process_stats.pid = getpid();
  process_stats.cpu_secs = cputime_ns/SEC_ACC;
  process_stats.priority = priority;
  strncpy(process_stats.client_process_name, argv[0]+2, 15);
  memcpy(stats_ptr, &process_stats , sizeof(process_stats));
  setpriority(PRIO_PROCESS, process_stats.pid, priority);
  process_stats.priority = getpriority(PRIO_PROCESS , process_stats.pid);
  // strncpy(process_stats.client_process_name, argv[0]+2, 15);
  tv.tv_sec = 0;
  tv.tv_nsec = sleeptime_ns;
  // process_stats.cpu_secs= 0;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &temp);
  total_cputime = 0;
  while (1) {
    // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    nanosleep(&tv, &tv);
    process_stats.counter++;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    do {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    temp_cputime = (end.tv_sec - start.tv_sec)*SEC_ACC;
    temp_cputime += (end.tv_nsec - start.tv_nsec);
    } while (temp_cputime < (cputime_ns* 0.95) );
    // total_cputime += temp_cputime;
    total_cputime = (end.tv_sec - temp.tv_sec)*SEC_ACC;
    total_cputime += (end.tv_nsec - temp.tv_nsec);
    process_stats.cpu_secs = total_cputime/SEC_ACC;
    if (DEBUG) {
    printf("Writing to shared memory at address %p\n", shm);
    printf("Writing to shared memory at address %p\n", stats_ptr);
    }
    // nanosleep(&tv, &tv);
    memcpy(stats_ptr, &process_stats , sizeof(process_stats));
    // process_stats.counter++;
    // nanosleep(&tv, &tv);
  }
  exit(0);
}
