#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

void perror_exit(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

int main()
{
  char fn[] = "fifo.fifo";
  char out[100] = "this is fifo!", in[100];
  int rf, wf;

  if (mkfifo(fn, S_IRWXU) != 0)
    perror_exit("mkfifo error");

  // nonblocking open read end to avoid deadlock
  if ((rf = open(fn, O_RDONLY | O_NONBLOCK)) < 0)
    perror_exit("open error on read end");

  // no need to nonblocking open write end
  if ((wf = open(fn, O_WRONLY)) < 0)
    perror_exit("open error on write end");

  if (write(wf, out, strlen(out) + 1) < 0)
    perror_exit("write");

  if (read(rf, in, sizeof(in)) < 0)
    perror_exit("read");

  printf("read '%s' from FIFIO\n", in);

  if (close(rf) == -1)
    perror_exit("close read end");
  if (close(wf) == -1)
    perror_exit("close write end");

  if (unlink(fn) == -1)
    perror_exit("unlink");

  return 0;
}