/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

typedef struct {
	bool git;
	bool clang;
	bool editor;
} flag_t;

typedef struct {
	bool ncurses;
	bool raylib;
	bool stb;
	bool uthash;
	bool linenoise;
} libmap_t;

typedef enum { MIT, GPL, BSD, UNL, _LICENCE_COUNT_ } licence_t;

typedef enum { POSIX, SIMPLE, GNU, LIBRARY, FASM, _STYLE_COUNT_ } style_t;

typedef struct {
	libmap_t libraries;
	licence_t licence;
	flag_t flags;
	style_t style;

	char *project;
	char *name;
	char *editor;
} manifest_t;

int create_project(manifest_t manifest);

#endif // YAIT_H
