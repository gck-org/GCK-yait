/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include "../include/yait.h"
#include "licences/licences.h"
#include "util.h"

int create_project(manifest_t manifest)
{
	int status;
	char buffer[BUFSIZ], *main_source;

	status = mkdir_p(manifest.project);
	if (status)
		return 1;

	status = chdir(manifest.project);
	if (status)
		return 1;

	flast = true;
	switch (manifest.licence) {
	case MIT:
		cfprintf("COPYING", "%s", MIT_txt);
		break;
	case GPL:
		cfprintf("COPYING", "%s", GPL_3_0_txt);
		break;
	case BSD:
		cfprintf("COPYING", "%s", BSD_3_Clause_txt);
		break;
	case UNL:
		cfprintf("COPYING", "%s", UNLICENSE_txt);
		break;
	default:
		abort();
	}

	if (!manifest.git) {
		fprintf(stderr, "Initializing git reposity");
		system("git init --quiet");
		fprintf(stderr, ", done.\n");
	}

	if (manifest.open_editor) {
		snprintf(buffer, BUFSIZ, "nvim %s", main_source);
		system(buffer);
	}

	return 0;
}
