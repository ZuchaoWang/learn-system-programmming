#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/utsname.h>

int main()
{
  struct utsname buf;

  errno = 0;
  if (uname(&buf) != 0)
  {
    perror("uname error: ");
    exit(EXIT_FAILURE);
  }

  printf("sysname = %s\n", buf.sysname);
  printf("nodename = %s\n", buf.nodename);
  printf("release = %s\n", buf.release);
  printf("version = %s\n", buf.version);
  printf("machine = %s\n", buf.machine);
}