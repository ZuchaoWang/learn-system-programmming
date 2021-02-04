#include <stdio.h>
#include <stdlib.h>

void check_ptr(int* ptr, int num);

int main()
{
  int *ptr;
  int num, sum = 0;

  num = 4;
  printf("we will allocate memory to %d elements\n", num);
  ptr = (int*)malloc(num*sizeof(int));
  check_ptr(ptr, num);

  num = 8;
  printf("we will re-allocate memory to %d elements\n", num);
  ptr = (int*)realloc(ptr, num*sizeof(int));
  check_ptr(ptr, num);

  printf("we will free allocated memory\n");
  free(ptr);

  return 0;
}

void check_ptr(int* ptr, int num) {
  if (ptr == NULL) {
    printf("memory allocation failed!\n");
    exit(1);
  } else {
    printf("memory allocation succeeded!\n");
    for (int i=0; i<num; i++) ptr[i] = i+1;
    for (int i=0; i<num; i++) printf("%d ", ptr[i]);
    printf("\n");
  }
}