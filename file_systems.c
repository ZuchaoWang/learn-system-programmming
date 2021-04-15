#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

int main()
{
  struct statvfs buf;

  if (statvfs(".", &buf) == -1)
  {
    perror("statvfs:");
    exit(EXIT_FAILURE);
  }

  // ref: https://linux.die.net/man/2/statvfs
  printf("block size in bytes: f_frsize = %ld\n", buf.f_frsize);

  printf("blocks total: f_blocks = %u\n", buf.f_blocks);
  printf("blocks available: f_bfree = %u\n", buf.f_bfree);
  printf("blocks available to unpriviledged processes: f_bavail = %u\n", buf.f_bavail);

  printf("inodes total: f_files = %u\n", buf.f_files);
  printf("inodes available: f_ffree = %u\n", buf.f_ffree);
  printf("inodes available to unpriviledged processes: f_favail = %u\n", buf.f_favail);

  printf("file system id: f_fsid = %lu\n", buf.f_fsid);
  printf("mount flags: f_flag = %lu\n", buf.f_flag);
  printf("filename length limit: f_namemax = %lu\n", buf.f_namemax);

  return EXIT_SUCCESS;
}