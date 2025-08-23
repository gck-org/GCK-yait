/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#include "../include/yait.h"
#include "util.h"
#include "contents.h"

#define emit_progress_file                        \
	fprintf(stderr, "Created files %d\r", a); \
	a++

int create_project(manifest_t manifest)
{
	int status, a = 1;
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
		emit_progress_file;
		cfprintf("README", "%s", manifest.project);
		emit_progress_file;

		snprintf(buffer, BUFSIZ, "%s.c", manifest.project);
		cfprintf(buffer, "");
		fprintf(stderr, "Created files %d, done.\n", a);
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
			"#include <stdio.h>\n\nint main() {\n\tputs(\"Hei!\");\n\treturn 0;\n}");
		cfprintf("TOOLS/build.sh",
			 "#!/bin/sh\n\ncc SOURCE/main.c -o %s",
			 manifest.project);
		break;
	default:
		abort();
	}

	return 0;
}
