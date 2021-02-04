#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
  fork();
  printf("hello!\n");
  exit(0);
  printf("byte!\n");
}