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
  key_t key;
  if((key = ftok("./msgqueue.c", 65)) == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  int id;
  if ((id = msgget(key, 0666 | IPC_CREAT)) == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  return id;
}

void run_sender() {
  int msgid;
  msgid = get_msgid();

  struct msg_buffer msg;
  msg.type = 1;
  sprintf(msg.content, "%s", "hello world!");

  printf("message sent: %s\n", msg.content);
  fflush(stdout);

  if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
    perror("msgsnd");
    exit(EXIT_FAILURE);
  }
}

void run_receiver() {
  int msgid;
  msgid = get_msgid();

  struct msg_buffer msg;
  msg.type = 1;

  if (msgrcv(msgid, &msg, sizeof(msg), 1, 0) == -1) {
    perror("msgrcv");
    exit(EXIT_FAILURE);
  }

  printf("message received: %s\n", msg.content);
  fflush(stdout);

  if (msgctl(msgid, IPC_RMID, NULL) == -1) {
    perror("msgctl");
    exit(EXIT_FAILURE);
  }
}

int main()
{
  int status;

  // fork sender
  int cpid1 = fork();
  if (cpid1 == -1) {
    perror("fork sender");
    exit(EXIT_FAILURE);
  } else if (cpid1 == 0) {
    run_sender();
    exit(EXIT_SUCCESS);
  }

  // wait for sender to finish
  if (waitpid(cpid1, &status, 0) == -1) {
    perror("waitpid for sender");
    exit(EXIT_FAILURE);
  }

  // fork receiver
  int cpid2 = fork();
  if (cpid2 == -1) {
    perror("fork receiver");
    exit(EXIT_FAILURE);
  } else if (cpid2 == 0) {
    run_receiver();
    exit(EXIT_SUCCESS);
  }

  // wait for receiver to finish
  if (waitpid(cpid2, &status, 0) == -1) {
    perror("waitpid for receiver");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
