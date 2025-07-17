#ifndef CORE_H
#define CORE_H

#include <stdbool.h>

typedef enum {
  BSD3,
  GPLv3,
  MIT,
  UNLICENCE,
} licence_t;

typedef enum {
  RAYLIB,
  WINAPI,
  cURL,
} lib_t;

typedef struct {
  bool git;
  bool clang_format;
  licence_t licence;
  char *name;
} format_t;

#endif
