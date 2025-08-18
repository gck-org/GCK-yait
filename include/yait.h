/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

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

#define HAS_LIBRARY(libs, lib) ((libs) & (lib))
#define ADD_LIBRARY(libs, lib) ((libs) |= (lib))
#define REMOVE_LIBRARY(libs, lib) ((libs) &= ~(lib))

int create_project(manifest_t manifest);
int mkdir_p(const char *path);
int create_file_with_content(const char *path, const char *format, ...);

#endif // YAIT_H
