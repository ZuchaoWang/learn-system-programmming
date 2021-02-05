#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

static void daemonhandler()
{
  pid_t pid;
  pid = fork();
  if (pid < 0) exit(EXIT_FAILURE);
  if (pid > 0) exit(EXIT_SUCCESS);

  if (setsid() < 0) exit(EXIT_FAILURE);
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  pid = fork();
  if (pid < 0) exit(EXIT_FAILURE);
  if (pid > 0) exit(EXIT_SUCCESS);

  umask(0);
  chdir("/");

  int x;
  for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
  {
    close(x);
  }

  openlog("daemons", LOG_PID, LOG_DAEMON);
}

int main()
{
  daemonhandler();

  syslog(LOG_NOTICE, "daemon has started");
  sleep(20);
  syslog(LOG_NOTICE, "daemon has terminated");
  closelog();
  
  return EXIT_SUCCESS;
}