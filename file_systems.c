#include <stdio.h>
#include <sys/statvfs.h>

int main()
{
  struct statvfs buf;

  // first arg of statvfs is the path of any file in the file system
  if (statvfs(".", &buf) == -1)
  {
    perror("statvfs error:");
  }
  else
  {
    // ref: https://linux.die.net/man/2/statvfs
    printf("block size in bytes: f_frsize = %ld\n", buf.f_frsize);
    printf("blocks available: f_bavail = %u\n", buf.f_bavail);
    printf("blocks total: f_blocks = %u\n", buf.f_blocks);
    printf("inodes available: f_favail = %u\n", buf.f_favail);
    printf("inodes total: f_files = %u\n", buf.f_files);
    printf("filename length limit: f_namemax = %lu\n", buf.f_namemax);
    printf("file system id: f_fsid = %lu\n", buf.f_fsid);
  }
}