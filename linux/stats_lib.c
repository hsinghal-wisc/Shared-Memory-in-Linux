#include <sys/shm.h>
#include "stats.h"
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>

#define true 1
#define false 0

// char sem_name[32] ="gautamsingh";
sem_t * shmem_sem;
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

extern stats_t * stats_ptr;
char num[32];
stats_t* stats_init(key_t key) {
  int shmid, int_key;
  char *shm, *s;
  int pos;
  char sem_name[32] ="gautamsingh";
  int count = 0;
  // char num[32];
  // stats_t * stats_ptr;
  // char num[32];
  int page_size = getpagesize();
  // printf("making call to shmget\n");
  if ((shmid = shmget(key, page_size, 0666)) < 0) {
    // printf("Error: shmget\n");
    return NULL;
  }
  // printf("returned from call to shmget\n");
  int_key = (int) key;
  strcat(sem_name, itoa(int_key, num, 10) );
  // printf("Unqiue semaphore name is %s\n",sem_name);
  shmem_sem = sem_open(sem_name, 0);
  if (SEM_FAILED == shmem_sem) {
  // printf("sem_open failed errno: %d\n",errno);
    sem_close(shmem_sem);
    return NULL;
  }
  #if 0
  printf("making call to shmget\n");
  if ((shmid = shmget(key, page_size, 0666)) < 0) {
    // printf("Error: shmget\n");
    return NULL;
  }
  // printf("returned from call to shmget\n");
  #endif
  // printf("making call to shmatt\n");
  /*
  int_key = (int) key;
  strcat(sem_name, itoa( int_key,num, 10));
  printf("Unqiue semaphore name is %s\n",sem_name);
    shmem_sem = sem_open(sem_name, 0);
  if (SEM_FAILED == shmem_sem)  {
   printf("sem_open failed errno: %d\n",errno);
   }
  */                                         
  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    // printf("Error: shmat\n");
    sem_close(shmem_sem);
    return NULL;
  }
  // printf("returned from call to shmat\n");
  stats_ptr = (stats_t *) shm;
  // printf("before sem_wait\n");
  sem_wait(shmem_sem);
  // printf("stats_ptr: %p\n",stats_ptr);
  // printf("stats_ptr->pid: %d\n",stats_ptr->pid);
  while (stats_ptr->pid && count < 16)
    stats_ptr++, count++;
  if (count >= 16)
    stats_ptr = 0;
  // printf("sem_wait sem_post\n");
  sem_post(shmem_sem);
  // printf("after sem_post\n");
  // printf("Address: %p  size: %d\n", stats_ptr,sizeof(stats_ptr));
  return stats_ptr;
}
int stats_unlink(key_t key) {
  int int_key;
  char sem_name[32] = "gautamsingh";
  int_key = (int) key;
  strcat(sem_name, itoa(int_key, num, 10));
  // printf("Unqiue semaphore name is %s\n",sem_name);
  shmem_sem = sem_open(sem_name, 0);
  if (SEM_FAILED == shmem_sem) {
    // printf("sem_open failed errno: %d\n",errno);
    return -1;
  }
  sem_wait(shmem_sem);
  if (stats_ptr)
  memset(stats_ptr, 0, sizeof(stats_t));
  sem_post(shmem_sem);
  sem_close(shmem_sem);
  return 0;
}
