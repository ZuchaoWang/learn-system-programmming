#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *threadfun(void *vargp)
{
  sleep(2);
  printf("hello threading\n");
  return NULL;
}

int main()
{
  pthread_t thread_id;
  printf("before threading\n");
  pthread_create(&thread_id, NULL, threadfun, NULL);
  pthread_join(thread_id, NULL); // block and wait for thread to terminate
  printf("after threading\n");
  return 0;
}