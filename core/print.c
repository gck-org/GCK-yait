#include "print.h"
#include <stdarg.h>
#include <stdio.h>

int printfn(char *format, ...) {
  int len;
  va_list args;
  va_start(args, format);
  fprintf(stderr, "yait: ");
  len = vfprintf(stderr, format, args);
  putchar('\n');
  va_end(args);
  return len;
}
