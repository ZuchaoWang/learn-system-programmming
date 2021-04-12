#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main()
{
  char fn[] = "fifo.fifo";
  char out[100] = "this is fifo!", in[100];
  int rf, wf;

  if (mkfifo(fn, S_IRWXU) != 0)
  {
    perror("mkfifo error");
  }

  if ((rf = open(fn, O_RDONLY | O_NONBLOCK)) < 0)
  {
    perror("open error on read end");
  }

  if ((wf = open(fn, O_WRONLY)) < 0)
  {
    perror("open error on write end");
  }

  if (write(wf, out, strlen(out) + 1) < 0)
  {
    perror("write");
  }

  if (read(rf, in, sizeof(in)) < 0)
  {
    perror("read");
  }

  printf("reading '%s' from FIFIO\n", in);

  close(wf);
  close(rf);

  unlink(fn);

  return 0;
}