#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/utsname.h>

int main()
{
  struct utsname buff;

  errno = 0;
  if (uname(&buff) != 0)
  {
    perror("uname error: ");
    exit(EXIT_FAILURE);
  }

  printf("sysname = %s\n", buff.sysname);
  printf("nodename = %s\n", buff.nodename);
  printf("release = %s\n", buff.release);
  printf("version = %s\n", buff.version);
  printf("machine = %s\n", buff.machine);
}