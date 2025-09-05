/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

typedef enum { MIT, GPL, BSD, UNL, LCOUNT } licence_t;
typedef enum { MAKE, CMAKE, AUTOTOOLS, BARE, BCOUNT } built_t;

typedef struct {
	licence_t licence;
	built_t build;

	bool lib;

	bool git;
	bool flat;
	bool open_editor;

	struct {
		bool build_nob;
		bool tools_format;
		bool tools_Cleanup;
	} extra;

	char *project;
	char *author;
	char *editor;
} manifest_t;

int create_project(manifest_t manifest);

#endif // YAIT_H
