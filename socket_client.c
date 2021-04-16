#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main()
{
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    perror_exit("socket");

  struct sockaddr_in svaddr;
  memset(&svaddr, 0, sizeof(svaddr));
  svaddr.sin_family = AF_INET;
  svaddr.sin_port = htons(PORT_NUM);
  svaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(sockfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1)
    perror_exit("connect");

  char svaddrstr[ADDRSTRLEN];
  get_addrstr((struct sockaddr *)&svaddr, sizeof(svaddr), svaddrstr, ADDRSTRLEN);
  printf("connected to: %s\n", svaddrstr);
  printf("please type message, enter to send\n");
  fflush(stdout);

  char buf[BUFSIZ];
  ssize_t numRead, numWritten;
  while (fgets(buf, BUFSIZ, stdin) != NULL)
  {
    numRead = strlen(buf);
    // notice here's a lack of timeout detection
    // so client will hang when server closed after connection
    if ((numWritten = write(sockfd, buf, numRead)) != numRead)
    {
      perror_exit("write");
    }
  }
  if (numRead == -1)
    perror_exit("read");

  return EXIT_SUCCESS;
}