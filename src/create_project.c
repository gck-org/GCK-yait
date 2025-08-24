/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>

#include "../include/yait.h"
#include "posix_contents.h"
#include "library_contents.h"
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

	switch (manifest.style) {
	case SIMPLE:
		cfprintf(
			"Makefile",
			".POSIX:\nCC ::= gcc\nCFLAGS ::= -Wall --std=c23 -Wpedantic\n\nall: %s",
			manifest.project);
		cfprintf("README", "%s", manifest.project);

		snprintf(buffer, BUFSIZ, "%s.c", manifest.project);

		flast = true;
		cfprintf(buffer, "");

		snprintf(buffer, BUFSIZ, "%s.c", manifest.project);
		main_source = str_dup(buffer);
		break;

	case POSIX:
		cfprintf("Makefile", "");
		cfprintf("configure", configure);
		cfprintf(".clang-format", "");
		cfprintf("README", readme, manifest.name);
		cfprintf("src/main.c", "");

		snprintf(buffer, BUFSIZ, "include/%s.h", manifest.project);
		cfprintf(buffer, "");

		snprintf(buffer, BUFSIZ, "man/%s.1", manifest.project);
		cfprintf(buffer, ".\\\" %s.1 - Manual page for %s",
			 manifest.project, manifest.project);

		flast = true;
		cfprintf("doc/WHATNEXT", what_next);

		fprintf(stderr, "Changing permissions 1");
		if (chmod("configure", 0755) == -1) {
			fprintf(stderr, "\n");
			perror("chmod");
			return 1;
		}
		fprintf(stderr, ", done.\n");

		main_source = "src/main.c";
		break;
	case FASM:
		snprintf(buffer, BUFSIZ, "%s.TXT", manifest.project);
		for (int i = 0; buffer[i] != '\0'; ++i)
			buffer[i] = toupper((unsigned char)buffer[i]);
		cfprintf(
			buffer,
			"<https://github.com/tgrysztar/fasm/blob/master/FASM.TXT>");
		cfprintf(
			"SOURCE/main.c",
			"#include <stdio.h>\n\nint main() {\n\tputs(\"Who's that behind you?\");\n\treturn 0;\n}");
		flast = true;
		cfprintf("TOOLS/build.sh",
			 "#!/bin/sh\n\ncc SOURCE/main.c -o %s",
			 manifest.project);

		fprintf(stderr, "Changing permissions 1");
		if (chmod("TOOLS/build.sh", 0755) == -1) {
			fprintf(stderr, "\n");
			perror("chmod");
			return 1;
		}
		fprintf(stderr, ", done.\n");

		main_source = "SOURCE/main.c";
		break;
	case GNU:
		cfprintf("AUTHORS", "%s", manifest.name);
		cfprintf("INSTALL",
			 "autoreconf -i\n./configure\nmake\nmake install");
		cfprintf("COPYING", "%s", "(undefined)");

		cfprintf(
			"NEWS",
			"%s (0.1) unstable; urgency=low\n\n  * Initial release. It compiles!\n   - Nothing else.\n",
			manifest.project);
		cfprintf(
			"README",
			"This is the README file for the %s distribution.\n %s does a thing.",
			manifest.project, manifest.project);
		cfprintf(
			"configure.ac",
			"AC_INIT([%s], [0.1], [you@example.com])\nAM_INIT_AUTOMAKE([-Wall -Wextra foreign])\nAC_PROG_CC\nAC_CONFIG_FILES([Makefile src/Makefile man/Makefile])\nAC_OUTPUT");
		cfprintf(
			"src/main.c",
			"#include <stdio.h>\n\nint main(void) {\n\tputs(\"Who's that behind you?\");\n\treturn 0;\n}");

		time_t t = time(NULL);
		struct tm tm_info = *localtime(&t);
		char date[11];
		strftime(date, sizeof(date), "%Y-%m-%d", &tm_info);

		snprintf(buffer, BUFSIZ, "man/%s.1", manifest.project);
		flast = true;
		cfprintf(
			buffer,
			".TH %s 1 \"%s\" \"0.1\" \"User Commands\"\n.SH NAME\n%s \\- a program that does a thing\n.SH SYNOPSIS\n.B %s\n.SH DESCRIPTION\nThis is a program that does a thing.\n.SH AUTHOR\nWritten by %s.",
			manifest.project, date, manifest.project,
			manifest.project, manifest.name);

		main_source = "src/main.c";
		break;
	case LIBRARY:
		snprintf(buffer, BUFSIZ, "include/%s.h", manifest.project);
		char *file_name_buffer = str_dup(buffer);

		snprintf(buffer, BUFSIZ, "%s", manifest.project);
		for (int i = 0; buffer[i] != '\0'; ++i)
			buffer[i] = toupper((unsigned char)buffer[i]);

		cfprintf(file_name_buffer, "#ifndef %s\n#define %s\n#endif",
			 buffer, buffer);

		snprintf(buffer, BUFSIZ, "src/%s.c", manifest.project);
		main_source = str_dup(buffer);
		cfprintf(buffer, "");

		cfprintf("Makefile", makefile_lib, manifest.project,
			 manifest.project);

		cfprintf("README", "%s", manifest.project);

		break;
	default:
		abort();
	}

	if (manifest.libraries.ncurses) {
		fprintf(stderr, "Pulling ncurses");
		system("git submodule add --quiet https://github.com/mirror/ncurses");
		fprintf(stderr, ", done.\n");
	}
	if (manifest.libraries.raylib) {
		fprintf(stderr, "Pulling raylib");
		system("git submodule add --quiet https://github.com/raysan5/raylib");
		fprintf(stderr, ", done.\n");
	}
	if (manifest.libraries.stb) {
		fprintf(stderr, "Pulling stb");
		system("git submodule add --quiet https://github.com/nothings/stb");
		fprintf(stderr, ", done.\n");
	}
	if (manifest.libraries.uthash) {
		fprintf(stderr, "Pulling uthash");
		system("git submodule add --quiet https://github.com/troydhanson/uthash");
		fprintf(stderr, ", done.\n");
	}
	if (manifest.libraries.linenoise) {
		fprintf(stderr, "Pulling linenoise");
		system("git submodule add --quiet https://github.com/antirez/linenoise");
		fprintf(stderr, ", done.\n");
	}

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

	if (!manifest.flags.git) {
		fprintf(stderr, "Initializing git reposity");
		system("git init --quiet");
		fprintf(stderr, ", done.\n");
	}

	if (manifest.flags.editor) {
		snprintf(buffer, BUFSIZ, "nvim %s", main_source);
		system(buffer);
	}

	return 0;
}
