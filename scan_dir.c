#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

void perror_exit(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

char *get_username(uid_t uid)
{
  struct passwd *pwent = getpwuid(uid);
  if (pwent == NULL)
    perror_exit("getpwuid:");
  return pwent->pw_name;
}

char *get_groupname(gid_t gid)
{
  struct group *grpent = getgrgid(gid);
  if (grpent == NULL)
    perror_exit("getgrgid:");
  return grpent->gr_name;
}

char* get_time(time_t t) {
  char* str = ctime(&t);
  if (str[strlen(str)-1] == '\n')
    str[strlen(str)-1] = '\0';
  return str;
}

char get_filetype(mode_t perm)
{
  switch (perm & S_IFMT)
  {
  case S_IFREG:
    return '-';
  case S_IFDIR:
    return 'd';
  case S_IFCHR:
    return 'c';
  case S_IFBLK:
    return 'b';
  case S_IFLNK:
    return 'l';
  case S_IFIFO:
    return 'p';
  case S_IFSOCK:
    return 's';
  default:
    perror_exit("get_filetype:");
    return '?';
  }
}

char *get_fileperm(mode_t perm)
{
  static char buf[10];
  snprintf(buf, 10, "%c%c%c%c%c%c%c%c%c",
           (perm & S_IRUSR) ? 'r' : '-',
           (perm & S_IWUSR) ? 'w' : '-',
           (perm & S_IXUSR) ? ((perm & S_ISUID) ? 's' : 'x') : ((perm & S_ISUID) ? 'S' : '-'),
           (perm & S_IRGRP) ? 'r' : '-',
           (perm & S_IWGRP) ? 'w' : '-',
           (perm & S_IXGRP) ? ((perm & S_ISGID) ? 's' : 'x') : ((perm & S_ISGID) ? 'S' : '-'),
           (perm & S_IROTH) ? 'r' : '-',
           (perm & S_IWOTH) ? 'w' : '-',
           (perm & S_IXOTH) ? ((perm & S_ISVTX) ? 't' : 'x') : ((perm & S_ISVTX) ? 'T' : '-'));
  return buf;
}

void scan_dir(const char *dirpath)
{
  DIR *dirp;
  struct dirent *dp;
  char filepath[BUFSIZ];
  struct stat statbuf;

  if ((dirp = opendir(dirpath)) == NULL)
  {
    perror_exit("opendir:");
  }

  for (;;)
  {
    errno = 0; // to distinguish error from end-of-directory
    dp = readdir(dirp);
    if (dp == NULL)
    { // detect error and end-of-directory
      if (errno != 0)
        perror_exit("readdir");
      else
        break; // end-of-directory
    }
    else if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
    {
      continue; // skip . and ..
    }
    else
    {
      if (snprintf(filepath, BUFSIZ, "%s/%s", dirpath, dp->d_name) < 0)
      {
        perror_exit("sprintf:");
      }
      if (stat(filepath, &statbuf) == -1)
      {
        perror_exit("stat:");
      }

      printf("%c%s\t%d\t%s\t%s\t%lld\t%s\t%s\n",
             get_filetype(statbuf.st_mode),
             get_fileperm(statbuf.st_mode),
             statbuf.st_nlink,
             get_username(statbuf.st_uid),
             get_groupname(statbuf.st_gid),
             statbuf.st_size,
             get_time(statbuf.st_mtime),
             dp->d_name);
    }
  }

  if (closedir(dirp) == -1)
    perror_exit("closedir");
}

int main()
{
  scan_dir("./");
  return EXIT_SUCCESS;
}