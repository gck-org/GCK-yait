#ifndef CORE_H
#define CORE_H

#include <stdbool.h>

typedef enum {
  OPENBSD3,
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
  bool nogit;
  licence_t licence;
  char *name;
  lib_t libraries[];
} format_t;

#endif
