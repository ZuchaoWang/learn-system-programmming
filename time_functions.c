#include <stdio.h>
#include <time.h>
#include <unistd.h>

void wait_kb()
{
  printf("start waiting ...\n");
  sleep(1);
  printf("end waiting ...\n");
}

int main()
{
  clock_t c0, c1;
  time_t t0, t1;
  c0 = clock();
  t0 = time(NULL);
  wait_kb();
  c1 = clock();
  t1 = time(NULL);
  double cdur = ((double)(c1 - c0)) / CLOCKS_PER_SEC;
  double tdur = t1 - t0;
  printf("the clock time is %fs\n", cdur);
  printf("the elapsed time is %fs\n", tdur);
}