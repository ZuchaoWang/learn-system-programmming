#include <stdio.h>

int main()
{
  char buf[BUFSIZ];
  FILE *fp = popen("ls", "r");
  while (fgets(buf, BUFSIZ, fp) != NULL)
  {
    printf("ls: %s", buf);
  }
  pclose(fp);
  return 0;
}