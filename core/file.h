#ifndef FILE_H
#define FILE_H

#include "e.h"
#include <unistd.h>

#define take(x, ...)                                                           \
  dir(x, ##__VA_ARGS__);                                                       \
  chdir(x);

error_t touch(char *, char *, ...);
error_t dir(char *, ...);

#endif
