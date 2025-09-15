/* Copyright (C) GCK
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../config.h"
#include "standard.h"

int parse_standard_options(void (*usage)(int), int argc, char **argv)
{
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			usage(0);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i], "--version") == 0) {
			printf("%s %s %d\nCopyright (C) %d %s.\n%s\nThis is "
			       "free software: "
			       "you are free to change and redistribute "
			       "it.\nThere is NO "
			       "WARRNTY, to the extent permitted by law.\n",
			       PROGRAM, VERSION, COMMIT, YEAR, AUTHORS,
			       LICENSE_LINE);
			exit(EXIT_SUCCESS);
		}
	}
	return HELP_REQUESTED;
}
