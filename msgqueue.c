#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// System-V message queue

struct msg_buffer {
  long type;
  char content[BUFSIZ];
};

int get_msgid() {
  key_t key = ftok("msqqueue", 65);
  return msgget(key, 0666 | IPC_CREAT);
}

void run_sender() {
  int msgid;
  msgid = get_msgid();

  struct msg_buffer msg;
  msg.type = 1;

  printf("please write data to send:\n");
  fflush(stdout);
  fgets(msg.content, BUFSIZ, stdin);

  msgsnd(msgid, &msg, sizeof(msg), 0);

  exit(EXIT_SUCCESS);
}

void run_receiver() {
  int msgid;
  msgid = get_msgid();

  struct msg_buffer msg;
  msg.type = 1;

  msgrcv(msgid, &msg, sizeof(msg), 1, 0);

  printf("message received:\n");
  printf("%s", msg.content);
  fflush(stdout);

  msgctl(msgid, IPC_RMID, NULL);

  exit(EXIT_SUCCESS);
}

int main()
{
  int status;

  // fork child1 to send
  int cpid1 = fork();
  if (cpid1 == -1) {
    perror("fork sender");
    exit(EXIT_FAILURE);
  } else if (cpid1 == 0) {
    run_sender();
  }

  // wait for child1 to finish
  if (waitpid(cpid1, &status, 0) == -1) {
    perror("waitpid for sender");
    exit(EXIT_FAILURE);
  }

  // fork child2 to receive
  int cpid2 = fork();
  if (cpid2 == -1) {
    perror("fork receiver");
    exit(EXIT_FAILURE);
  } else if (cpid2 == 0) {
    run_receiver();
  }

  // wait for child2 to finish
  if (waitpid(cpid2, &status, 0) == -1) {
    perror("waitpid for receiver");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
