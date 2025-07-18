#include "file.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int
touch (char *path, char *format, ...)
{
  FILE *fp = fopen (path, "w");
  if (!fp)
    {
      return 0;
    }

  else
    {
      va_list args;
      va_start (args, format);
      vfprintf (fp, format, args);
      va_end (args);
    }

  fclose (fp);
  return 0;
}

int
dir (char *format, ...)
{
  va_list args;
  va_start (args, format);

  char path[1024];
  vsnprintf (path, sizeof (path), format, args);

  va_end (args);

  if (mkdir (path, 0777) < 0)
    {
      return errno;
    }

  return 0;
}

int
take (const char *dirname)
{
  int err = dir ("%s", dirname);
  if (err)
    {
      return err;
    }
  if (chdir (dirname) != 0)
    {
      return errno;
    }
  return 0;
}
