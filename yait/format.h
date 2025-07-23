// Copyright (C) 2025 vx_clutch ( owestness@gmail.com )
// See end of file for extended copyright information.

#ifndef FORMAT_H
#define FORMAT_H

#include <stdbool.h>

/* License type enumeration */
typedef enum
{
  BSD3,      /* BSD 3-Clause License */
  GPLv3,     /* GNU General Public License v3 */
  MIT,       /* MIT License */
  UNLICENCE, /* Unlicense */
} licence_t;

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
} flags_t;

/* Project configuration structure */
typedef struct
{
  licence_t licence;     /* License type for the project */
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

/* yait is yet another init tool.
 * Copyright (C) 2025 vx-clutch
 *
 * This file is part of yait.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions, and the following disclaimer in the documentation
 * or other materials provided with the distribution.
 * 3. Neither the name of vx-clutch nor the names of its contributors may be
 * used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
