#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define MYSTACKSIZE 40960

pthread_mutex_t m;

void *hello(void *max)
{
  int i;
  struct timeval tp;

  pthread_mutex_lock(&m);
  printf("hello\n");
  pthread_mutex_unlock(&m);
}

void *world(void *max)
{
  int i;
  struct timeval tp;
 
  pthread_mutex_lock(&m);
  printf("world\n");
  pthread_mutex_unlock(&m);
}

main()
{
  int max = 50, max1 = 100, max2 = 200, i;
  pthread_attr_t attr;
  pthread_t *th1, *th2;
  void *st1, *st2;
  size_t sz;
  int policy;
  struct timeval tp;

  pthread_mutex_init(&m, NULL);

  pthread_attr_init(&attr);
  st1 = (void *) malloc(MYSTACKSIZE);
  pthread_attr_setstacksize(&attr, MYSTACKSIZE);
  pthread_attr_setstack(&attr, st1, MYSTACKSIZE);

  pthread_attr_getstacksize(&attr, &sz);

  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

  th1 = (pthread_t *) malloc(sizeof(pthread_t));
  if (pthread_create(th1, &attr, hello, &max1)) {
    perror("error creating the first thread");
    exit(1);
  }
 
  st2 = (void *)malloc(MYSTACKSIZE);
  pthread_attr_setstacksize(&attr, MYSTACKSIZE);
  pthread_attr_setstack(&attr, st2, MYSTACKSIZE);
  th2 = (pthread_t *) malloc(sizeof(pthread_t));
  if (pthread_create(th2, &attr, world, &max2)) {
    perror("error creating the second thread");
    exit(1);
  }


  pthread_join(*th1, 0);
  pthread_join(*th2, 0);
}