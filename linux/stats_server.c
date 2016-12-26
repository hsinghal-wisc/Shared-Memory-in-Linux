#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include "stats.h"

#define true 1
#define false 0

char sem_name[32] ="gautamsingh";

void reverse(char str[], int length) {
  char temp;
  int start = 0;
  int end = length -1;
  while (start < end) {
    temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

char* itoa(int num, char* str, int base) {
  int i = 0;
  int isNegative = false;
  if (num == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }
  if (num < 0 && base == 10) {
    isNegative = true;
    num = -num;
  }
  while (num != 0) {
    int rem = num % base;
    str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
    num = num/base;
  }
  if (isNegative)
    str[i++] = '-';
  str[i] = '\0';
  reverse(str, i);
  return str;
}

int shmid = 0;
void sig_handler(int signo) {
  //  signal(SIGINT, SIG_IGN);
  if (sem_unlink(sem_name)) {
    perror("semaphore unlink failed");
  }
  if (signo == SIGINT) {
    shmctl(shmid, IPC_RMID, NULL);
    // printf(" shared memory removed : %d\n", shmid);
  }
  exit(0);
}
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


int main(int argc, char *argv[]) {
  char c, num[32], *shm, *s;
  //  int shmid;
  int page_size, loop, int_key;
  key_t key;
  stats_t *client_stat;
  sem_t * shm_sem;
  if (argc < 3 || argc > 3) {
    // printf(" Invalid Arguments\n");
    // printf(" Usage: stat_server -k key\n");
    return 1;
  }
  #if 0
  if (myAtoi(argv[2], &int_key)) {
    key = (key_t) int_key;
  } else {
    // printf("Illegal key\n");
    return 1;
  }
  #else
  int_key = atoi(argv[2]);
  key = (key_t) int_key;
  #endif
  if (signal(SIGINT, sig_handler) == SIG_ERR)
  printf("SIGINT install error\n");
  strcat(sem_name, itoa(int_key, num, 10) );
  // printf("Unqiue semaphore name is %s\n", sem_name);
  if ((shm_sem = sem_open(sem_name, O_CREAT |O_EXCL,
S_IRUSR | S_IWUSR |S_IRGRP | S_IWGRP, 1)) == SEM_FAILED) {
    // perror("semaphore initilization");
    exit(1);
  }
  page_size = getpagesize();
  // printf("Page size: %d\n", page_size);
  if ((shmid = shmget(key, page_size, IPC_CREAT | 0666)) < 0) {
    // perror("shmget");
    exit(1);
  }
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    // perror("shmat");
    exit(1);
  }
  // printf(" shared memory attached : %d\n", shmid);
  // if(DEBUG)
    // printf(" Shared memory address : %p\n", shm);
  int server_iter = 0;
  while (1) {
    server_iter++;
    // sleep(1);
  s = shm;
  client_stat = s;
  for (loop = 0; loop < 16 && client_stat->pid; loop++, client_stat++) {
    // server_iter++;
  printf("%d ", server_iter);
  printf("%d ", client_stat->pid);
  // printf("stats_client ", );
  printf("%s ", client_stat->client_process_name);
  printf("%d ", client_stat->counter);
  printf("%.2f ", client_stat->cpu_secs);
  printf("%d\n", client_stat->priority);
  }
  printf("\n");
  sleep(1);
  }
  if (sem_unlink("gautamsingh_sem")) {
    perror("semaphore unlink failed");
  }
  exit(0);
}
 /*  struct sigaction sigact, old_sigact;
  sigset_t sigset;
  siginfo_t siginfo; 
  sigemptyset(&sigact.sa_mask);
  // sigaddset( &sigact.sa_mask, SIGUSR2 );
  sigact.sa_flags = SA_SIGINFO;
  sigact.sa_handler = catcher;
  sigact.sa_sigaction = sig_handler2; 
  sigaction( SIGINT, &sigact, NULL );
  */
