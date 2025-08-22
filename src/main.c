/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

// Usage: yait [OPTION]... <PROJECT>

#define _POSIX_C_SOURCE 200809L // popen extention
#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "../include/yait.h"
#include "standard.h"
#include "util.h"

#define print_option(option, description) \
	printf("        %-20s %-20s\n", option, description)

char *str_dup(char *s)
{
	char *new = malloc(strlen(s) + 1);
	if (!new)
		return NULL;
	strcpy(new, s);
	return new;
}

static void usage(int status)
{
	if (status != 0) {
		puts("Try 'yait --help' for more information.\n");
		return;
	}

	puts("Usage: yait [OPTION]... <PATH>");
	puts("Creates a C project with opinionated defaults");
	puts("When only given the first argument it will detect your name\n");
	puts("Mandatory arguments to long options are mandatory for short options too");
	print_option("--no-git", "Do not inititize git reposity");
	print_option("--clang", "Add clang-format files and tooling");
	print_option("-L <licence>", "Set licence");
	print_option("-l <library>", "Add a library");
	print_option("-n <name>", "Set the name of the project");
	print_option(
		"--style=<style>",
		"Pick from a list of built in styles. This list can be found by passing 'list'");
	puts("    --help     display this help text and exit\n");
	puts("    --version  output version information and exit\n");
}

static int parse_arguments(manifest_t *conf, int argc, char **argv)
{
	int opt;

	// clang-format off
	static struct option long_opts[] = {
		{ "style", required_argument, 0, 's' },
		{ "no-git", no_argument, 0, 'g' },
		{ "clang", no_argument, 0, 'c' },
		{ 0, 0, 0, 0 } };
	// clang-format on

	// TODO(vx-clutch): lL
	while ((opt = getopt_long(argc, argv, "s:gcn", long_opts, NULL)) !=
	       -1) {
		switch (opt) {
		case 's':
			if (strcmp(optarg, "list") == 0) {
				fputs("", stderr);
			}
			break;
		case 'g':
			conf->flags.git = false;
			break;
		case 'c':
			conf->flags.clang = true;
			break;
		case 'n':
			conf->name = str_dup(optarg);
			break;
		default:
			return 1;
		}
	}

	if (optind >= argc) {
		fputs("error: missing required positional argument", stderr);
		return 1;
	}
	conf->project = str_dup(argv[optind]);

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
		.style = SIMPLE,

		.libraries.ncurses = false,
		.libraries.raylib = false,
		.libraries.stb = false,
		.libraries.uthash = false,
		.libraries.linenoise = false,

		.flags.git = true,
		.flags.clang = false,
	};

	status = parse_standard_options(usage, argc, argv);
	if (status != 0 && status != HELP_REQUESTED)
		return fprintf(stderr, "error: %s\n", strerror(status)),
		       EXIT_FAILURE;

	status = parse_arguments(&manifest, argc, argv);
	if (status) {
		return EXIT_FAILURE;
	}

	get_name(&manifest.name);
	status = create_project(manifest);

	char buffer[PATH_MAX];
	getcwd(buffer, PATH_MAX);
	fprintf(stderr, "Created %s at\n %s\n", manifest.project, buffer);

	return EXIT_SUCCESS;
}
