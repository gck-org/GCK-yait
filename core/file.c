#include "file.h"
#include "e.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

bool
contains_percent (const char *s)
{
  while (*s)
    {
      if (*s == '%')
        return true;
      s++;
    }
  return false;
}

error_t
touch (char *path, char *format, ...)
{
  error_t err = { 0 };
  err.null = true;

  FILE *fp = fopen (path, "w");
  if (!fp)
    {
      err.null = false;
      err.status = errno;
      err.src = strerror (errno);
      return err;
    }

  if (contains_percent (format))
    {
      va_list args;
      va_start (args, format);
      vfprintf (fp, format, args);
      va_end (args);
    }
  else
    {
      fputs (format, fp);
    }

  fclose (fp);
  return err;
}

error_t
dir (char *format, ...)
{
  error_t err = { 0 };
  err.null = true; // success by default

  va_list args;
  va_start (args, format);

  char path[1024];
  vsnprintf (path, sizeof (path), format, args);

  va_end (args);

  if (mkdir (path, 0777) < 0)
    {
      err.null = false;
      err.status = errno;
      err.src = strerror (errno);
    }

  return err;
}
