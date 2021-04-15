#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define CUR_FILENAME "./producer_consumer_sem.c"
#define BUFLEN 8
#define SEM_LOCK 0
#define SEM_SPACE 1
#define SEM_CHAR 2
#define MESSAGE "abcdefghijklmnopqrstuvwxyz\n"

struct shmseg
{
  int in;
  int out;
  char buffer[BUFLEN];
};

// union semun {
//   int val;
//   struct semid_ds *buf;
//   unsigned short *array;
// };

int get_shmid()
{
  key_t key;
  if ((key = ftok(CUR_FILENAME, 66)) == -1)
  {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  int id;
  if ((id = shmget(key, sizeof(struct shmseg), OBJ_PERMS | IPC_CREAT)) == -1)
  {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  return id;
}

struct shmseg *open_shm(shmid)
{
  struct shmseg *addr;
  if ((addr = shmat(shmid, NULL, 0)) == (void *)-1)
  {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  return addr;
}

void init_shm(struct shmseg *seg)
{
  seg->in = 0;
  seg->out = 0;
  memset(seg->buffer, 0, BUFLEN);
}

void unlink_shm(shmid)
{
  if (shmctl(shmid, IPC_RMID, NULL) == -1)
  {
    perror("shmctl-IPC_RMID");
    exit(EXIT_FAILURE);
  }
}

int get_semid()
{
  key_t key;
  if ((key = ftok(CUR_FILENAME, 67)) == -1)
  {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  int id;
  if ((id = semget(key, 3, OBJ_PERMS | IPC_CREAT)) == -1)
  {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  return id;
}

void init_sem(int semid)
{
  unsigned short vals[] = {1, BUFLEN, 0};
  union semun arg;
  arg.array = vals;
  if (semctl(semid, 0, SETALL, arg) == -1)
  {
    perror("semctl-SETALL");
    exit(EXIT_FAILURE);
  }

  // to ensure initialization success
  if (semctl(semid, 0, GETALL, arg) == -1)
  {
    perror("semctl-GETALL");
    exit(EXIT_FAILURE);
  }
  printf("semaphore initialized: lock=%d, space=%d, char=%d\n", arg.array[SEM_LOCK], arg.array[SEM_SPACE], arg.array[SEM_CHAR]);
}

void unlink_sem(int semid)
{
  if (semctl(semid, 0, IPC_RMID, NULL) == -1)
  {
    perror("semctl-IPC_RMID");
    exit(EXIT_FAILURE);
  }
}

void wait_sem(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = sem_num;
  sops.sem_op = -1;
  sops.sem_flg = 0;
  if (semop(semid, &sops, 1) == -1)
  {
    perror("semop-wait");
    exit(EXIT_FAILURE);
  }
}

void signal_sem(int semid, int sem_num)
{
  struct sembuf sops;
  sops.sem_num = sem_num;
  sops.sem_op = 1;
  sops.sem_flg = 0;
  if (semop(semid, &sops, 1) == -1)
  {
    perror("semop-signal");
    exit(EXIT_FAILURE);
  }
}

void sleep_rand()
{
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = (double)(rand()) / RAND_MAX * 0.1f * 1e9;
  nanosleep(&ts, NULL);
}

void run_producer()
{
  srand(getpid());
  sleep_rand();

  printf("%s", "[P]");
  fflush(stdout);

  int shmid, semid;
  struct shmseg *seg;
  shmid = get_shmid();
  seg = open_shm(shmid);
  semid = get_semid();

  int i, len;
  char msg[] = MESSAGE, c;
  len = strlen(msg);
  for (i = 0; i < len; i++)
  {
    // read message
    c = msg[i];  
    // critical section
    wait_sem(semid, SEM_SPACE);
    wait_sem(semid, SEM_LOCK);
    seg->buffer[seg->in] = c;
    seg->in = (seg->in + 1) % BUFLEN;
    // printing here just to correctly show process execution order
    // normally it should be put in position *** for performance
    if (c != '\n') {
      printf("%c", c - 'a' + 'A');
      fflush(stdout);
    }
    signal_sem(semid, SEM_LOCK);
    signal_sem(semid, SEM_CHAR);
    // position ***
    // random wait
    sleep_rand();
  }
}

void run_consumer()
{
  srand(getpid());
  sleep_rand();

  printf("%s", "[C]");
  fflush(stdout);

  int shmid, semid;
  struct shmseg *seg;
  shmid = get_shmid();
  seg = open_shm(shmid);
  semid = get_semid();

  char c;
  while (1)
  {
    // critical section
    wait_sem(semid, SEM_CHAR);
    wait_sem(semid, SEM_LOCK);
    c = seg->buffer[seg->out];
    seg->out = (seg->out + 1) % BUFLEN;
    // printing here just to correctly show process execution order
    // normally it should be put in position *** for performance
    if (c != '\n') {
      printf("%c", c);
      fflush(stdout);
    }
    signal_sem(semid, SEM_LOCK);
    signal_sem(semid, SEM_SPACE);
    // write message
    if (c == '\n')
      break;
    // position ***
    // random wait
    sleep_rand();
  }
  printf("%c", '\n');
}

int main()
{
  int shmid, semid;
  struct shmseg *seg;
  shmid = get_shmid();
  seg = open_shm(shmid);
  semid = get_semid();

  // initialization
  init_shm(seg);
  init_sem(semid);

  int status;
  // fork producer
  int cpid1 = fork();
  if (cpid1 == -1)
  {
    perror("fork producer");
    exit(EXIT_FAILURE);
  }
  else if (cpid1 == 0)
  {
    run_producer();
    exit(EXIT_SUCCESS);
  }

  // fork consumer
  int cpid2 = fork();
  if (cpid2 == -1)
  {
    perror("fork consumer");
    exit(EXIT_FAILURE);
  }
  else if (cpid2 == 0)
  {
    run_consumer();
    exit(EXIT_SUCCESS);
  }

  // wait for producer to finish
  if (waitpid(cpid1, &status, 0) == -1)
  {
    perror("waitpid for producer");
    exit(EXIT_FAILURE);
  }

  // wait for consumer to finish
  if (waitpid(cpid2, &status, 0) == -1)
  {
    perror("waitpid for consumer");
    exit(EXIT_FAILURE);
  }

  // unlink
  unlink_shm(shmid);
  unlink_sem(semid);
  return EXIT_SUCCESS;
}