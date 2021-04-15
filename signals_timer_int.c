#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#define INTERVAL 500

volatile sig_atomic_t flag_time = 0;
volatile sig_atomic_t flag_int = 0;

void perror_exit(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void sigalrmhandler(int signum)
{
  flag_time = 1;
}

void siginthandler(int signum)
{
  flag_int = 1;
}

int main()
{
  sigset_t origMask, blockMask;
  struct sigaction sa;

  // block signals
  sigemptyset(&blockMask);
  sigaddset(&blockMask, SIGALRM);
  sigaddset(&blockMask, SIGINT);
  if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
    perror_exit("sigprocmask block");

  // register sigalrmhandler
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = sigalrmhandler;
  if (sigaction(SIGALRM, &sa, NULL) == -1)
    perror_exit("sigaction alrm");

  // register siginthanlder
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = siginthandler;
  if (sigaction(SIGINT, &sa, NULL) == -1)
    perror_exit("sigaction int");

  // set realtime timer to periodically trigger SIGALRM
  struct itimerval it_val;
  it_val.it_value.tv_sec = INTERVAL / 1000;
  it_val.it_value.tv_usec = (INTERVAL * 1000) % 1000000;
  it_val.it_interval = it_val.it_value;
  if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)
    perror_exit("setitimer");

  // waiting for signals
  for (;;)
  {
    if (sigsuspend(&origMask) == -1 && errno != EINTR)
      perror_exit("sigsuspend");
    if (flag_time == 1)
    {
      printf("timer went off\n");
      flag_time = 0;
    }
    if (flag_int == 1)
    {
      printf("interrupted\n");
      break;
    }
  }

  if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
    perror_exit("sigprocmask restore");

  return EXIT_SUCCESS;
}