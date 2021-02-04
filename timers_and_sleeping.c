// Javascript equivalent of this example is
//   setInterval(sigalrmhandler, INTERVAL);

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define INTERVAL 500

void sigalrmhandler(int);

int main()
{
  struct itimerval it_val;
  // let sigalrmhandler handle SIGALRM signal sent by setitimer
  if (signal(SIGALRM, sigalrmhandler) == SIG_ERR)
  {
    perror("unable to catch SIGALRM:");
    exit(EXIT_FAILURE);
  }

  // ref: https://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html

  // delay to first timer interrupt
  it_val.it_value.tv_sec = INTERVAL / 1000;
  it_val.it_value.tv_usec = (INTERVAL * 1000) % 1000000;
  // interval between later successive timer interrupts
  it_val.it_interval = it_val.it_value;

  // setitimer with ITIMER_REAL creates a realtime timer
  // when timeout it gives a SIGALRM signal
  if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)
  {
    perror("error calling setitimer:");
    exit(1);
  }

  for (;;)
  {
    // sleep until receive signal, ref: https://linux.die.net/man/2/pause
    pause();
  }
}

void sigalrmhandler(int signum)
{
  printf("timer went off\n");
}