#ifndef FILE_H
#define FILE_H

#include "e.h"
#include <unistd.h>

error_t take(const char *dirname);

error_t touch(char *, char *, ...);
error_t dir(char *, ...);

#endif
