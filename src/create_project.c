/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <unistd.h>
#include <stdio.h>

#include "../include/yait.h"
#include "util.h"
#include "contents.h"

char buffer[BUFSIZ];

int create_project(manifest_t manifest)
{
	mkdir_p(manifest.project);
	chdir(manifest.project);

	if (manifest.style == SIMPLE) {
		/* This only works if the source
			files's root name is the same as the target on all of the Makefile becuase of how it checks for files. */
		cfprintf(
			"Makefile",
			".POSIX:\nCC ::= gcc\nCFLAGS ::= -Wall --std=c23 -Wpedantic\n\nall: %s",
			manifest.project);
		fprintf(stderr, "Created files 1\r");
		cfprintf("README", "%s", manifest.project);
		fprintf(stderr, "Created files 2\r");

		snprintf(buffer, BUFSIZ, "%s.c", manifest.project);
		cfprintf(buffer, "");
		fputs("Created files 3, done.\n", stderr);
		/* We exit early because the simple flag is an overridding flag. */
		return 0;
	}

	cfprintf("Makefile", "");
	cfprintf("configure", configure);
	cfprintf(".clang-format", "");
	cfprintf("README", readme, manifest.name);
	cfprintf("src/main.c", "");

	snprintf(buffer, BUFSIZ, "include/%s.h", manifest.project);
	cfprintf(buffer, "");

	snprintf(buffer, BUFSIZ, "man/%s.1", manifest.project);
	cfprintf(buffer, "");

	cfprintf("doc/WHATNEXT", what_next);

	return 0;
}
