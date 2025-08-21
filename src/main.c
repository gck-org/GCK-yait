/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

// Usage: yait [OPTION]... <PROJECT>

#include <getopt.h>
#include <pwd.h>
#define _POSIX_C_SOURCE 200809L // popen extention
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/yait.h"
#include "standard.h"
#include "util.h"

#define print_option(option, description) \
	fprintf(stderr, "        %-20s %-20s\n", option, description)

static void usage(int status)
{
	if (status != 0) {
		fputs("Try 'yait --help' for more information.\n", stderr);
		return;
	}

	fputs("Usage: yait [OPTION]... <PATH>", stderr);
	fputs("Creates a C project with opinionated defaults.", stderr);
	fputs("When only given the first argument it will detect your name.\n",
	      stderr);
	fputs("Mandatory arguments to long options are mandatory for short options too",
	      stderr);
	print_option("--posix", "Enable POSIX compliance");
	print_option("--git", "Do not inititize git reposity");
	print_option("--clang", "Add clang-format files and tooling");
	print_option("-L <licence>", "Set licence");
	print_option("-l <library>", "Add a library");
	print_option("-C", "Use C++");
	print_option("--GNU",
		     "Add GNU argument parsing: --version, and --help");
	print_option("-S, --simple", "Enable simple mode");
	fputs("    --help     display this help text and exit", stderr);
	fputs("    --version  output version information and exit", stderr);
}

static int parse_arguments(manifest_t *conf, int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "L:l:CS")) != -1) {
		switch (opt) {
		case 'L':
			conf->licence = TOlicence(optarg);
			break;
		case 'l':
			break;
		case 'C':
			conf->flags.cc = true;
			break;
		case 'S':
			conf->flags.simple = true;
			break;
		default:
			fprintf(stderr, "Usage: %s [--help]\n", argv[0]);
			return 1;
		}
	}

	return 0;
}

int get_name(char **output)
{
	FILE *pipe;
	char buffer[128];
	size_t output_len = 0;

	*output = NULL; // make sure it's NULL before realloc

	pipe = popen("git config user.name", "r");
	if (!pipe)
		exit(EXIT_FAILURE);

	while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
		size_t chunk_len = strlen(buffer);
		char *new_output = realloc(*output, output_len + chunk_len + 1);
		if (!new_output) {
			free(*output);
			pclose(pipe);
			exit(EXIT_FAILURE);
		}
		*output = new_output;
		memcpy((*output) + output_len, buffer, chunk_len);
		output_len += chunk_len;
		(*output)[output_len] = '\0';
	}
	pclose(pipe);
	return 0;
}

int main(int argc, char **argv)
{
	int status;
	manifest_t manifest = {
		.project = "Project",
		.name = "author",
		.licence = UNL,
		.libraries.ncurses = false,
		.libraries.raylib = false,
		.libraries.stb = false,
		.libraries.uthash = false,
		.libraries.linenoise = false,

		.flags.posix = false,
		.flags.git = true,
		.flags.clang = false,
		.flags.lib = false,
		.flags.cc = false,
		.flags.gnu = true,
		.flags.simple = false,
	};

	status = parse_standard_options(usage, argc, argv);
	if (status != 0 && status != HELP_REQUESTED)
		return fprintf(stderr, "error: %s\n", strerror(status)),
		       EXIT_FAILURE;

	parse_arguments(&manifest, argc, argv);

	get_name(&manifest.name);
	status = create_project(manifest);

	return EXIT_SUCCESS;
}
