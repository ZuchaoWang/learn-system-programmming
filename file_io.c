#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *wf, *rf;
  char fn[] = "file_io.txt";

  if ((wf = fopen(fn, "w")) == NULL) {
    printf("error: can't open file!\n");
    exit(1);
  }

  if ((rf = fopen(fn, "r")) == NULL) {
    printf("error: can't open file!\n");
    exit(1);
  }

  // write to file
  int num1;
  printf("type a number: ");
  scanf("%d", &num1);
  fprintf(wf, "%d", num1);
  fflush(wf); // need fflush to ensure writting to kernel, otherwise need fclose
  printf("number written down!\n");

  // read from file
  int num2;
  fscanf(rf, "%d", &num2);
  printf("number read out: %d\n", num2);

  // close all
  fclose(wf);
  fclose(rf);
  remove(fn); // remove is generic, while unlink is unix-specific

  return 0;
}