/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

typedef enum { MIT, GPL, BSD, UNL, _LICENCE_COUNT_ } licence_t;

typedef struct {
	licence_t licence;

	bool lib;
	bool exe;

	bool git;
	bool autotools;
	bool cmake;
	bool make;
	bool bare;
	bool flat;
	bool open_editor;

	struct {
		bool nob;
		bool clang_format;
		bool Cleanup;
	} extra;

	char *project;
	char *author;
	char *editor;
} manifest_t;

int create_project(manifest_t manifest);

#endif // YAIT_H
