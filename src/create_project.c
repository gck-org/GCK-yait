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
#include "contents.h"

char buffer[BUFSIZ];

int create_project(manifest_t manifest)
{
	mkdir_p(manifest.project);
	chdir(manifest.project);

	if (manifest.flags.simple) {
		cfprintf(
			"Makefile",
			".POSIX:\nCC ::= gcc\nCFLAGS ::= -Wall --std=c23 -Wpedantic\n\nall: %s",
			manifest.project);
		cfprintf("README", "%s", manifest.project);

		snprintf(buffer, BUFSIZ, "%s.c", manifest.project);
		cfprintf(buffer, "");
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
