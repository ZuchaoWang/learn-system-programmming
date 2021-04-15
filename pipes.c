#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void perror_exit(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

int main()
{
  int status;
  int filedes[2];
  if (pipe(filedes) == -1)
    perror_exit("pipe");

  // fork writer, exec "ls -l"
  int cpid1 = fork();
  if (cpid1 == -1)
  {
    perror_exit("fork writer");
  }
  else if (cpid1 == 0)
  {
    if (close(filedes[0]) == -1)
      perror_exit("close writer 1");
    if (filedes[1] != STDOUT_FILENO)
    {
      if (dup2(filedes[1], STDOUT_FILENO) == -1)
        perror_exit("dup writer");
      if (close(filedes[1]) == -1)
        perror_exit("close writer 2");
    }
    execlp("ls", "ls", "-l", (char *)NULL);
    perror_exit("exec writer");
  }

  // fork reader, exec "wc"
  int cpid2 = fork();
  if (cpid2 == -1)
  {
    perror_exit("fork reader");
  }
  else if (cpid2 == 0)
  {
    if (close(filedes[1]) == -1)
      perror_exit("close reader 1");
    if (filedes[0] != STDIN_FILENO)
    {
      if (dup2(filedes[0], STDIN_FILENO) == -1)
        perror_exit("dup reader");
      if (close(filedes[0]) == -1)
        perror_exit("close reader 2");
    }
    execlp("wc", "wc", (char *)NULL);
    perror_exit("exec reader");
  }

  if (close(filedes[0]) == -1)
    perror_exit("close parent 1");

  // must close write end of pipe, so that reader reads EOF, then process and exit
  // otherwise reader can block waiting for EOF, and subsequent waitpid for reader never returns
  if (close(filedes[1]) == -1) 
    perror_exit("close parent 2");

  // wait for writer to finish
  if (waitpid(cpid1, &status, 0) == -1)
    perror_exit("waitpid for writer");

  // wait for reader to finish
  if (waitpid(cpid2, &status, 0) == -1)
    perror_exit("waitpid for reader");

  return EXIT_SUCCESS;
}