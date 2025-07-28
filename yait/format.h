/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef FORMAT_H
#define FORMAT_H

#include <stdbool.h>

/* License type enumeration */
typedef enum
{
  BSD3,      /* BSD 3-Clause License */
  GPLv3,     /* GNU General Public License v3 */
  MIT,       /* MIT License */
  UNlICENSE, /* Unlicense */
} license_t;

/* Library type enumeration - using bit flags for multiple selection */
typedef enum
{
  LIB_NONE = 0,        /* No libraries selected */
  LIB_RAYLIB = 1 << 0, /* Raylib game library */
  LIB_WINAPI = 1 << 1, /* Windows API */
  LIB_CURL = 1 << 2,   /* cURL library */
  /* Future libraries can be added here:
   * LIB_OPENGL   = 1 << 3,
   * LIB_SDL2     = 1 << 4,
   * LIB_GTK      = 1 << 5,
   * etc.
   */
} lib_flags_t;

/* Flag option type struct */
typedef struct
{
  bool GNU;
  bool git;
  bool clang_format;
  bool use_cpp;
} flags_t;

/* Project configuration structure */
typedef struct
{
  license_t license;     /* License type for the project */
  char *project;         /* Project name */
  char *name;            /* Author/creator name */
  lib_flags_t libraries; /* Selected libraries (bit field) */
  flags_t flag;          /* Flags */
} format_t;

/* Default values */
#define DEFAULT_LICENSE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true
#define DEFAULT_LIBRARIES LIB_NONE

/* Helper macros for library operations */
#define HAS_LIBRARY(libs, lib) ((libs) & (lib))
#define ADD_LIBRARY(libs, lib) ((libs) |= (lib))
#define REMOVE_LIBRARY(libs, lib) ((libs) &= ~(lib))
#define CLEAR_LIBRARIES(libs) ((libs) = LIB_NONE)

#endif
