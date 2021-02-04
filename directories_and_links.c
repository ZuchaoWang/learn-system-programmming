#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
  char dir_name[] = "./directories_and_links";
  char file_name[] = "./directories_and_links/file.txt";

  if (mkdir(dir_name, 0777) != 0) {
    printf("directory creation failed!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("directory created!\n");
  }

  FILE *fp;
  if ((fp = fopen(file_name, "w")) == NULL) {
    printf("file creation failed!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("file created!\n");
    fclose(fp);
  }

  sleep(10);

  if (unlink(file_name) != 0) {
    printf("file deletion failed!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("file deleted!\n");
  }

  if (rmdir(dir_name) != 0) {
    printf("directory deletion failed!\n");
    exit(EXIT_FAILURE);
  } else {
    printf("directory deleted!\n");
  }
}