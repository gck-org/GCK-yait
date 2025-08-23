/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#include "../include/yait.h"
#include "posix_contents.h"
#include "util.h"

int create_project(manifest_t manifest)
{
	int status;
	char buffer[BUFSIZ];

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
		break;
	case FASM:
		snprintf(buffer, BUFSIZ, "%s.txt", manifest.project);
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
		break;
	default:
		abort();
	}

	return 0;
}
