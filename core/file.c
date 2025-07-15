#include "file.h"
#include "e.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

error_t write(char *path, char *format, ...) {
  error_t err;
  va_list args;
  va_start(args, format);
  FILE *fp;
  fp = fopen(path, "w");
  if (!fp) {
    err.stat = errno;
    err.src = strerror(errno);
  }
  vfprintf(fp, format, args);
  fclose(fp);
  va_end(args);
  return err;
}
