/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

#include <string.h>

typedef enum {
	BSD3, 
	GPLv3, 
	MIT, 
	UNLICENCE, 
	LICENCE_HELP, 
} licence_t;

/* A bit field is used so that we can accomplish two things. (a) store lots of
   libraries without taxing memory; and (b) a dynamic array is not neccescary.
 */
typedef enum {
	LIB_NONE = 0,
	LIB_RAYLIB = 1 << 0,
	LIB_NCURSES = 1 << 1,
	LIB_CURL = 1 << 2,
	LIB_COUNT_,
	LIB_HELP,
} lib_flags_t;

typedef struct {
	bool GNU;
	bool git;
	bool clang_format;
	bool use_cpp;
} flags_t;

typedef struct {
	licence_t licence;
	char *project;
	char *name;
	lib_flags_t libraries;
	flags_t flag;
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
	return LIB_COUNT_; /* bad case */
}

#endif // YAIT_H
