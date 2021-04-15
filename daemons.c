#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

void perror_exit(char *msg)
{
  perror(msg); // this cannot really print anything because stderr connect to /dev/null
  exit(EXIT_FAILURE);
}

void become_daemon()
{
  pid_t pid;
  pid = fork();
  if (pid == -1)
    perror_exit("fork 1");
  else if (pid > 0)
    exit(EXIT_SUCCESS);

  if (setsid() == -1)
    perror_exit("setsid");

  pid = fork();
  if (pid == -1)
    perror_exit("fork 2");
  else if (pid > 0)
    exit(EXIT_SUCCESS);

  umask(0);
  chdir("/");

  int fd;
  for (fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
  {
    close(fd);
  }

  fd = open("/dev/null", O_RDWR);
  if (dup2(fd, STDIN_FILENO) != STDIN_FILENO)
    perror_exit("dup2 in");
  if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO)
    perror_exit("dup2 out");
  if (dup2(fd, STDERR_FILENO) != STDERR_FILENO)
    perror_exit("dup2 err");
  close(fd);
}

int main()
{
  become_daemon();

  sleep(20);

  return EXIT_SUCCESS;
}

// use "ps -Af | grep a.out" to find the daemon, assuming compiled executable is named "a.out"