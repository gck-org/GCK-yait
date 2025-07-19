#ifndef FORMAT_H
#define FORMAT_H

#include <stdbool.h>

/* License type enumeration */
typedef enum {
  BSD3,      /* BSD 3-Clause License */
  GPLv3,     /* GNU General Public License v3 */
  MIT,       /* MIT License */
  UNLICENCE, /* Unlicense */
} licence_t;

/* Library type enumeration */
typedef enum {
  RAYLIB,    /* Raylib game library */
  WINAPI,    /* Windows API */
  cURL,      /* cURL library */
} lib_t;

/* Project configuration structure */
typedef struct {
  bool git;              /* Whether to initialize git repository */
  bool clang_format;     /* Whether to create .clang-format file */
  licence_t licence;     /* License type for the project */
  char *project;         /* Project name */
  char *name;           /* Author/creator name */
} format_t;

/* Default values */
#define DEFAULT_LICENSE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true

#endif
