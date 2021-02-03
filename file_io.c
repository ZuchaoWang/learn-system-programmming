#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *fp;
  char filename[] = "file_io.txt";

  if ((fp = fopen(filename, "w")) == NULL) {
    printf("error: can't open file!\n");
    exit(1);
  }

  int num1;
  printf("type a number: ");
  scanf("%d", &num1);
  fprintf(fp, "%d", num1);
  // fflush(fp); // flush does not work, must fclose then fopen again
  printf("number written down!\n");
  fclose(fp);

  if ((fp = fopen(filename, "r")) == NULL) {
    printf("error: can't open file!\n");
    exit(1);
  }

  int num2;
  fscanf(fp, "%d", &num2);
  printf("number read out: %d\n", num2);

  fclose(fp);

  return 0;
}