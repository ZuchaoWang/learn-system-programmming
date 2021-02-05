#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  char *args[] = {"ls", NULL};
  execvp(args[0], args); // this replaces the current process
  printf("this line should not be printed\n");
  return 0;
}