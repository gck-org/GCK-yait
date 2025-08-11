/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

// Usage: yait [OPTION]... <PATH>

#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "../core/yait.h"
#include "../core/manifest.h"

#define print_option(option, description) \
	printf("        %-20s %-20s\n", option, description)

// clang-format off
static void
usage (int status)
{
  if (status != 0)
    {
      fprintf (stderr, "Try 'yait --help' for more information.\n");
      return;
    }

  printf ("Usage: yait [OPTION]... <PATH>\n");
  printf ("Creates a C project with opinionated defaults.\n");
  printf ("When only given the first argument it will detect your name.\n\n");
  printf ("Mandatory arguments to long options are mandatory for short options too\n");
  print_option ("-l, --licence=NAME", "Set licence (gpl, mit, bsd) [default: bsd]");
  print_option ("--lib=LIB", "Add a library to the project. You can list libraries with --lib=help.");
  print_option ("--use-cpp", "Uses the CPP language instead of C");
  print_option ("--git", "Initialize git repository");
  print_option ("--GNU", "Adds standard GNU argument parsing to your project");
  printf ("    --help     display this help text and exit\n");
  printf ("    --version  output version information and exit\n");
}
// clang-format on

static int parse_arguments(manifest_t *conf, int argc, char **argv)
{
	static struct option long_options[] = {
		{ "GNU", no_argument, 0, 'g' },
		{ "use-cpp", no_argument, 0, 'c' },
		{ "git", no_argument, 0, 'i' },
		{ "licence", required_argument, 0, 'l' },
		{ "lib", required_argument, 0, 'L' },
		{ "help", no_argument, 0, 'h' },
		{ 0, 0, 0, 0 }
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "gcil:L:h", long_options,
				  NULL)) != -1) {
		switch (opt) {
		case 'g':
			conf->flag.GNU = true;
			break;
		case 'c':
			conf->flag.use_cpp = true;
			break;
		case 'i':
			conf->flag.git = true;
			break;
		case 'l':
			if (strcmp(optarg, "bsd") == 0)
				conf->licence = BSD3;
			else if (strcmp(optarg, "gpl") == 0)
				conf->licence = GPLv3;
			else if (strcmp(optarg, "mit") == 0)
				conf->licence = MIT;
			else {
				fprintf(stderr, "Unknown licence: %s\n",
					optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'L':
			if (strcmp(optarg, "help"))
				fprintf(stderr, "raylib\nncurses\ncurl\n");
			else
				ADD_LIBRARY(conf->libraries, TOLibrary(optarg));
			break;
		case 'h':
			usage(0);
			exit(0);
		default:
			usage(1);
			exit(1);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Missing required path argument\n");
		usage(1);
		exit(EXIT_FAILURE);
	}

	conf->project = argv[optind++];

	if (optind < argc)
		conf->name = argv[optind++];

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
	manifest_t manifest = { 0 };

	status = parse_standard_options(usage, argc, argv);
	if (status != 0 && status != HELP_REQUESTED) {
		fprintf(stderr, "error: %s\n", strerror(status));
		return EXIT_FAILURE;
	}

	status = program_exists("git");
	if (status != 0) {
		fprintf(stderr, "git binary not present\n");
		return EXIT_FAILURE;
	}

	parse_arguments(&manifest, argc, argv);

	if (!manifest.name) {
		status = get_name(&manifest.name);
		if (status != 0 || !manifest.name || manifest.name[0] == '\0') {
			fprintf(stderr, "Could not determine user name\n");
			return EXIT_FAILURE;
		}
	}

	manifest.path = manifest.project;
	// TODO(vx-clutch): get name from path.

	status = create_project(manifest);
	return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
