#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

#define BUFLEN 8
#define MESSAGE "abcdefghijklmnopqrstuvwxyz\n"

static char buffer[BUFLEN] = {0, 0, 0, 0, 0, 0, 0, 0};
static int in = 0, out = 0, char_count = 0, space_count = BUFLEN;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t space_ok = PTHREAD_COND_INITIALIZER;
static pthread_cond_t char_ok = PTHREAD_COND_INITIALIZER;

void sleep_rand(unsigned int *pseed)
{
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = (double)(rand_r(pseed)) / RAND_MAX * 0.1f * 1e9;
  nanosleep(&ts, NULL);
}

void perror_exit(char *msg)
{
  perror(msg);
  _exit(EXIT_FAILURE);
}

void *run_producer(void *vargp)
{
  static unsigned int seed = 0;

  int i, len;
  char msg[] = MESSAGE, c;
  len = strlen(msg);
  for (i = 0; i < len; i++)
  {
    // read message
    c = msg[i];
    // critical section
    if (pthread_mutex_lock(&mtx) != 0)
      perror_exit("pthread_mutex_lock producer:");
    while (space_count == 0)
      if (pthread_cond_wait(&space_ok, &mtx) != 0)
        perror_exit("pthread_cond_wait producer:");
    buffer[in] = c;
    in = (in + 1) % BUFLEN;
    space_count--;
    char_count++;
    // printing here just to correctly show process execution order
    // normally it should be put in position *** for performance
    if (c != '\n')
    {
      printf("%c", c - 'a' + 'A');
      fflush(stdout);
    }
    if (pthread_mutex_unlock(&mtx) != 0)
      perror_exit("pthread_mutex_unlock producer:");
    if (pthread_cond_signal(&char_ok) != 0)
      perror_exit("pthread_cond_signal producer:");
    // position ***
    // random wait
    sleep_rand(&seed);
  }

  return NULL;
}

void *run_consumer(void *vargp)
{
  static unsigned int seed = 1;

  char c;
  while (1)
  {
    // critical section
    if (pthread_mutex_lock(&mtx) != 0)
      perror_exit("pthread_mutex_lock consumer:");
    while (char_count == 0)
      if (pthread_cond_wait(&char_ok, &mtx) != 0)
        perror_exit("pthread_cond_wait consumer:");
    c = buffer[out];
    out = (out + 1) % BUFLEN;
    char_count--;
    space_count++;
    // printing here just to correctly show process execution order
    // normally it should be put in position *** for performance
    if (c != '\n')
    {
      printf("%c", c);
      fflush(stdout);
    }
    if (pthread_mutex_unlock(&mtx) != 0)
      perror_exit("pthread_mutex_unlock consumer:");
    if (pthread_cond_signal(&space_ok) != 0)
      perror_exit("pthread_cond_signal consumer:");
    // write message
    if (c == '\n')
      break;
    // position ***
    // random wait
    sleep_rand(&seed);
  }
  printf("%c", '\n');

  return NULL;
}

int main()
{
  pthread_t thread_id1, thread_id2;
  if (pthread_create(&thread_id1, NULL, run_producer, NULL) != 0)
    perror_exit("pthread_create producer:");
  if (pthread_create(&thread_id2, NULL, run_consumer, NULL) != 0)
    perror_exit("pthread_create consumer:");
  if (pthread_join(thread_id1, NULL) != 0)
    perror_exit("pthread_join producer:");
  if (pthread_join(thread_id2, NULL) != 0)
    perror_exit("pthread_join consumer:");
  return EXIT_SUCCESS;
}