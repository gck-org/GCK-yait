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
	bool posix;
	bool git;
	bool clang;
	bool lib;
	bool cc;
	bool gnu;
	/* If this flag is set it will ignore: GNU, and enforce POSIX. */
	bool simple;
} flag_t;

typedef struct {
	bool ncurses;
	bool raylib;
	bool stb;
	bool uthash;
	bool linenoise;
} libmap_t;

typedef enum {
	MIT,
	GPL,
	BSD,
	UNL,
} licence_t;

typedef struct {
	libmap_t libraries;
	licence_t licence;
	flag_t flags;

	char *project;
	char *name;
} manifest_t;

int create_project(manifest_t manifest);
int mkdir_p(const char *path);
int cfprintf(const char *path, const char *format, ...);

#endif // YAIT_H
