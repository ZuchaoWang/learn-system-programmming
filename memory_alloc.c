#include <stdio.h>
#include <stdlib.h>

void check_ptr(int *ptr, int num)
{
  if (ptr == NULL)
  {
    printf("memory allocation failed!\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    for (int i = 0; i < num; i++)
      ptr[i] = i + 1;
    for (int i = 0; i < num; i++)
      printf("0x%x: %d\n", (unsigned int)(ptr + i), ptr[i]);
  }
}

int main()
{
  int *ptr;
  int num;

  num = 4;
  printf("memory allocation of %d elements\n", num);
  ptr = (int *)malloc(num * sizeof(int));
  check_ptr(ptr, num);

  num = 8;
  printf("memory re-allocation of %d elements\n", num);
  ptr = (int *)realloc(ptr, num * sizeof(int));
  check_ptr(ptr, num);

  free(ptr);

  return EXIT_SUCCESS;
}