/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licensed under the BSD-3-Clause licence.
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

	if (manifest.bare) {
		cfprintf("main.c", "");
		cfprintf(
			"Makefile",
			".POSIX:\nCC ::= gcc\nCFLAGS ::= -std=c23 -Wall -Wextra -Wpedantic\n\nall: %s\n\nclean\n\t$(RM) %s",
			manifest.project, manifest.project);
		cfprintf("README", "%s", manifest.project);
		goto bare_skip;
	}

	main_source = manifest.flat ? "main.c" : "src/main.c";
	cfprintf(main_source, "#include <stdio.h>\n"
			      "\n"
			      "int main()\n"
			      "{\n"
			      "\tputs(\"Hello, World!\");\n"
			      "\treturn 0;\n"
			      "}\n");
	char *upr_name = tostrupr(manifest.project);
	if (manifest.make) {
		cfprintf(
			"Makefile",
			"PREFIX = /usr/bin\n"
			"\n"
			"%s_SRCS := $(wildcard src/*.c)\n"
			"%s_OBJS := $(patsubst src/%.c,build/obj/%.o,$(%s_SRCS))"
			"\n"
			"%s := bin/%s"
			"\n"
			"-include config.mak\n"
			"\n"
			"ifeq ($(wildcard config.mak),)\n",
			upr_name, upr_name, upr_name, upr_name,
			manifest.project);
	}

bare_skip:
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
