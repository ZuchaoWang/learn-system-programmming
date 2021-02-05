#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  pid_t cpid;

  if (fork() == 0) // child
  { 
    exit(0);
  }
  else
  {
    // wait() blocks until child process terminates
    // ref: https://linux.die.net/man/2/wait
    cpid = wait(NULL);
    printf("current proc_id = %d\n", getpid());
    printf("child proc_id = %d\n", cpid);
  }

  return 0;
}