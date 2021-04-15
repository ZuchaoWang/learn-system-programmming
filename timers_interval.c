// Javascript equivalent of this example is
//   setInterval(sigalrmhandler, INTERVAL);

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define INTERVAL 500

volatile sig_atomic_t flag = 0;

void perror_exit(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void sigalrmhandler(int signum)
{
  flag = 1; // only set flag, let main deal with it
}

int main()
{
  if (signal(SIGALRM, sigalrmhandler) == SIG_ERR)
    perror_exit("signal");

  // delay to first timer interrupt
  struct itimerval it_val;
  it_val.it_value.tv_sec = INTERVAL / 1000;
  it_val.it_value.tv_usec = (INTERVAL * 1000) % 1000000;
  it_val.it_interval = it_val.it_value;

  // setitimer with ITIMER_REAL creates a realtime timer
  // when timeout it gives a SIGALRM signal
  if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)
    perror_exit("setitimer");

  for (;;)
  {
    pause(); // sleep until receive signal
    if (flag == 1)
    {
      printf("timer went off\n");
      flag = 0;
    }
  }
}