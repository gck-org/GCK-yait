#include "print.h"
#include <stdarg.h>
#include <stdio.h>

int
print_error_with_prefix (char *format, ...)
{
  int len;
  va_list args;
  va_start (args, format);
  fprintf (stderr, "yait: ");
  len = vfprintf (stderr, format, args);
  fprintf (stderr, "\n"); /* Use stderr consistently */
  va_end (args);
  return len;
}
