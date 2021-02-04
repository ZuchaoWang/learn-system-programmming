#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void siginthandler(int);

int main()
{
  signal(SIGINT, siginthandler);
  for (;;)
  {
    printf("sleeping ...\n");
    sleep(3);
  }
  return 0;
}

void siginthandler(int signum)
{
  printf("\ncaught signal %d, and now I'm exiting ...\n", signum);
  exit(1);
}