#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/ip.h>

#define BACKLOG 10
#define PORT_NUM 8080
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 20)

void perror_exit(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void handle_sigchld(int signum)
{
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0)
    continue;
  errno = saved_errno;
}

void get_addrstr(struct sockaddr *addr, socklen_t addrlen, char *addrstr, int addrstrlen)
{
  char hoststr[NI_MAXHOST];
  char servstr[NI_MAXSERV];

  if (getnameinfo(addr, addrlen, hoststr, NI_MAXHOST, servstr, NI_MAXSERV, NI_NUMERICSERV) == 0)
    snprintf(addrstr, addrstrlen, "(%s, %s)", hoststr, servstr);
  else
    snprintf(addrstr, addrstrlen, "(?UNKNOWN?)");
  addrstr[addrstrlen - 1] = '\0';
}

void handle_request(int cfd, char *addrstr)
{
  char buf[BUFSIZ];
  ssize_t numRead;
  printf("%s client connected\n", addrstr);
  while ((numRead = read(cfd, buf, BUFSIZ)) > 0)
  {
    printf("%s client message: %.*s", addrstr, (int)numRead, buf);
    fflush(stdout);
  }
  if (numRead == -1)
    perror_exit("read");
  printf("%s client disconnected\n", addrstr);
}

int main()
{
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handle_sigchld;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    perror_exit("sigaction");

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    perror_exit("socket");

  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    perror_exit("setsockopt");

  struct sockaddr_in svaddr;
  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sin_family = AF_INET;
  svaddr.sin_port = htons(PORT_NUM);
  // notice: INADDR_LOOPBACK is in host byte-order, not network byte-order
  // without htonl conversion, bind() will fail with "can't assign requested address"
  svaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (bind(sockfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1)
    perror_exit("bind");

  // notice: must listen, otherwise accept will fail
  if (listen(sockfd, BACKLOG) == -1)
    perror_exit("listen");

  char svaddrstr[ADDRSTRLEN];
  get_addrstr((struct sockaddr *)&svaddr, sizeof(svaddr), svaddrstr, ADDRSTRLEN);
  printf("server listening on %s\n", svaddrstr);
  fflush(stdout);

  int cfd;
  int pid;
  struct sockaddr_storage claddr;
  socklen_t claddrlen;
  char claddrstr[ADDRSTRLEN];
  for (;;)
  {
    claddrlen = sizeof(struct sockaddr_storage); // value-result
    cfd = accept(sockfd, (struct sockaddr *)&claddr, &claddrlen);
    if (cfd == -1)
    {
      perror("accept");
      continue;
    }

    pid = fork();
    if (pid == -1)
    {
      perror("fork");
      close(cfd);
    }
    else if (pid == 0)
    {
      close(sockfd);
      get_addrstr((struct sockaddr *)&claddr, claddrlen, claddrstr, ADDRSTRLEN);
      handle_request(cfd, claddrstr);
      exit(EXIT_SUCCESS);
    }
    else
    {
      close(cfd);
    }
  }
}