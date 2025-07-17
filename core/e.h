#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

typedef struct {
  bool null;
  int status;
  const char *src;
} error_t;

error_t unwrap(error_t);

#endif
