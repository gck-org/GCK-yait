/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#ifndef FORMAT_H
#define FORMAT_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef enum {
	BSD3, /* BSD 3-Clause Licence */
	GPLv3, /* GNU General Public Licence v3 */
	MIT, /* MIT Licence */
	UNLICENCE, /* Unlicence */
	LICENCE_HELP, /* Help case */
} licence_t;

/* A bit field is used so that we can accomplish two things. (a) store lots of
   libraries without taxing memory; and (b) a dynamic array is not neccescary.
 */
typedef enum {
	LIB_NONE = 0, /* No libraries selected */
	LIB_RAYLIB = 1 << 0, /* Raylib game library */
	LIB_NCURSES = 1 << 1, /* Windows API */
	LIB_CURL = 1 << 2, /* cURL library */
	LIB_COUNT_, /* Number of Libraries */
	LIB_HELP, /* Help case */
} lib_flags_t;

typedef struct {
	bool GNU;
	bool git;
	bool clang_format;
	bool use_cpp;
} flags_t;

typedef struct {
	licence_t licence; /* Licence type for the project */
	char *project; /* Project name */
	char *path; /* Path */
	char *name; /* Author/creator name ( if not provided infered on sanitize ) */
	lib_flags_t libraries; /* Selected libraries (bit field) */
	flags_t flag; /* Flags */
} manifest_t;

#define DEFAULT_CLANG_FORMAT true
#define DEFAULT_GIT_INIT true
#define DEFAULT_GNU false
#define DEFAULT_LIBRARIES LIB_NONE
#define DEFAULT_LICENCE BSD3

#define HAS_LIBRARY(libs, lib) ((libs) & (lib))
#define ADD_LIBRARY(libs, lib) ((libs) |= (lib))
#define REMOVE_LIBRARY(libs, lib) ((libs) &= ~(lib))

[[maybe_unused]] static lib_flags_t TOLibrary(char *src)
{
	if (strcmp(src, "raylib"))
		return LIB_RAYLIB;
	if (strcmp(src, "ncurse"))
		return LIB_NCURSES;
	if (strcmp(src, "ncurses"))
		return LIB_NCURSES;
	if (strcmp(src, "curl"))
		return LIB_CURL;
	if (strcmp(src, "help"))
		return LIB_HELP;
	fprintf(stderr, "could not find library %s", src);
	return LIB_COUNT_; /* bad case */
}

#endif
