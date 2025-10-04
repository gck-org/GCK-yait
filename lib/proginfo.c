/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <config.h>

#include "proginfo.h"

const char *prog_name = "";

void set_prog_name(char *name)
{
	prog_name = prog_name ? basename(name) : "";
}

void emit_try_help()
{
	printf("Try '%s --help' for more information\n", prog_name);
}

void emit_version()
{
	printf("\
%s %s %d\n\
Copyright (C) %d GCK.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRNTY, to the extent permitted by law.\n\
",
	       prog_name, VERSION, COMMIT, YEAR);
}

int parse_standard_options(int argc, char **argv, void (*print_help)(),
			   void (*print_version)())
{
	for (int i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			print_help();
			exit(EXIT_SUCCESS);
		} else if (!strcmp(argv[i], "--version")) {
			emit_version();
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}
