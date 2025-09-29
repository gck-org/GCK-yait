/* Copyright (C) vx-clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

// Usage: yait [OPTION]... <PROJECT>

#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include "../lib/proginfo.h"
#include "../lib/err.h"
#include "../include/yait.h"
#include "../config.h"
#include "name.h"

static struct option longopts[] = { { "author", required_argument, 0, 'a' },
				    { "licence", required_argument, 0, 'l' },
				    { 0, 0, 0, 0 } };

static int exit_status;

static void usage(int status);

int main(int argc, char **argv)
{
	int optc;
	int lose = 0;
	set_prog_name(argv[0]);

	exit_status = EXIT_SUCCESS;

	manifest_t manifest = {
		.author = get_name(),
		.editor = NULL,
		.licence = BSD,
		.project = "Project",
	};

	parse_standard_options(argc, argv, usage, emit_version);

	while ((optc = getopt_long(argc, argv, "a:l:E", longopts, NULL)) != -1)

		switch (optc) {
		case 'l':
			if (!strcmp(optarg, "list")) {
				printf("BSD\nGPL\nMIT\n");
				exit(EXIT_SUCCESS);
			}
			if (!strcmp(optarg, "GPL"))
				manifest.licence = GPL;
			else if (!strcmp(optarg, "MIT"))
				manifest.licence = MIT;
			else {
				printf("BSD\nGPL\nMIT\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'E':
			manifest.editor = getenv("EDITOR");
			break;
		default:
			lose = 1;
		}

	char *cwd = getcwd(NULL, 0);
	if (!cwd) {
		fatalfa(errno);
	}
	fprintf(stderr, "Created %s at\n %s\n", manifest.project, cwd);
	free(cwd);

	return exit_status;
}

static void usage(int status)
{
	if (status != 0) {
		puts("Try 'yait --help' for more information.");
		return;
	}

	printf("Usage: %s [OPTION]... [project-name]\n", PROGRAM);
	fputs("\
Generates an optionated C project.\n",
	      stdout);
	puts("");
	fputs("\
      --help                  display this help and exit\n\
      --version               display version information and eixt\n",
	      stdout);
	puts("");
	fputs("\
      -E                      Open $EDITOR after project creation (default false)\n\
      --author=NAME           Set the program author (default git username)\n\
      --licence=LICENCE       Set the program licence (default BSD)\n",
	      stdout);
}
