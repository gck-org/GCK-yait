/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
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

	cfprintf(
		"README",
		"%s ( short description )\n\nThis cool project actions adverbly.\n",
		manifest.project);

	if (manifest.build != BARE) {
		main_source = manifest.flat ? "main.c" : "src/main.c";
		cfprintf(main_source, "#include <stdio.h>\n"
				      "\n"
				      "int main()\n"
				      "{\n"
				      "\tputs(\"Hello, World!\");\n"
				      "\treturn 0;\n"
				      "}\n");
	}
	char *upr_name = tostrupr(manifest.project);
	switch (manifest.build) {
	case MAKE:
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
		break;
	case CMAKE:
		cfprintf("CMakeLists.txt",
			 "cmake_minimum_required(VERSION 3.16)\n"
			 "\n"
			 "project(%s\n"
			 "  VERSION 0.1.0\n"
			 "  LANGUAGES C)\n"
			 "\n"
			 "set(CMAKE_C_STANDARD 23)\n"
			 "set(CMAKE_C_STANDARD_REQUIRED ON)\n"
			 "set(CMAKE_C_EXTENSIONS OFF)\n"
			 "\n"
			 "include(GNUInstallDirs)\n"
			 "\n"
			 "add_executable(%s\n"
			 "  src/main.c\n"
			 ")\n"
			 "\n"
			 "install(TARGETS %s\n"
			 "  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}\n"
			 ")\n",
			 manifest.project, manifest.project, manifest.project);
		break;
	case AUTOTOOLS:
		cfprintf("configure.ac",
			 "AC_PREREQ([2.69])\n"
			 "AC_INIT([%s], [0.1], [bug-report@exmaple.com])\n"
			 "AM_INIT_AUTOMAKE([foreign -Wall])\n"
			 "AC_CONFIG_SRCDIR([src/main.c])\n"
			 "AC_CONFIG_HEADERS([config.h])"
			 "AC_PROG_CC\n"
			 "AC_CONFIG_FILES([Makefile src/Makefile])\n"
			 "AC_OUTPUT\n",
			 manifest.project);
		cfprintf("Makefile.am", "SUBDIRS = src\n");
		cfprintf("src/Makefile.am",
			 "bin_PROGRAMS = %s\n"
			 "%s_SOURCES = main.c\n",
			 manifest.project, manifest.project);
		cfprintf("bootstrap",
			 "#!/bin/sh\n"
			 "set -e\n"
			 "\n"
			 "autoreconf --install --verbose --force\n");
		break;
	case BARE:
		snprintf(buffer, BUFSIZ, "%s.c", manifest.project);
		cfprintf(buffer, "");
		main_source = str_dup(buffer);
		cfprintf("Makefile",
			 ".POSIX:\n"
			 "CC ::= gcc\n"
			 "CFLAGS ::= -std=c23 -Wall -Wextra -Wpedantic\n"
			 "\n"
			 "all: %s\n"
			 "\n"
			 "clean:\n"
			 "\t$(RM) %s",
			 manifest.project, manifest.project);
		break;
	case BCOUNT:
	default:
		abort();
	}

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
	case LCOUNT:
	default:
		abort();
	}

	if (!manifest.git) {
		fprintf(stderr, "Initializing git reposity");
		status = system("git init --quiet");
		if (status)
			fprintf(stderr, ", failed.\n");
		else
			fprintf(stderr, ", done.\n");
	}

	if (manifest.build == AUTOTOOLS) {
		fprintf(stderr, "Changing files permissions 1");
		struct stat st;
		if (stat("bootstrap", &st) == -1) {
			perror("stat failed");
			return 1;
		}
		mode_t new_mode = st.st_mode | S_IXUSR;
		if (chmod("bootstrap", new_mode) == -1) {
			perror("chmod failed");
			return 1;
		}
		fprintf(stderr, ", done.\n");
	}

	if (manifest.open_editor) {
		snprintf(buffer, BUFSIZ, "$EDITOR %s", main_source);
		status = system(buffer);
		if (status)
			fprintf(stderr, "Could not open editor");
	}

	return 0;
}
