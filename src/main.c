/* Copyright (C) vx-clutch
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
#include <stdarg.h>

#include "../include/yait.h"
#include "standard.h"
#include "util.h"
#include "../config.h"

#define print_option(option, description) \
	printf("        %-20s %-20s\n", option, description)

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
      --git                   Initialize git repository (default)\n\
      --no-git                Do not initialize git repository\n\
      --lib                   Make this a library\n\
      -l <licence>            Set licence. This list can be found by passing 'list'\n\
      -E                      Open $EDITOR after project creation\n\
      --autotools             Use the autotools build system\n\
      --cmake                 Use the cmake build system\n\
      --make                  Use the GNU make build system (default)\n\
      --bare                  Minimal C project structure\n\
      --flat                  All files in project root.\n\
      --extras=<arg1>,<arg2>  Extra build options, Pass list to list out options.\n",
	      stdout);
}

void print_lines(const char *first, ...)
{
	va_list args;
	const char *s;

	va_start(args, first);

	for (s = first; s != NULL; s = va_arg(args, const char *))
		printf("%s\n", s);

	va_end(args);
}

static inline int parse_extras_token(manifest_t *conf, const char *s)
{
	if (!strcmp(s, "list")) {
		print_lines("nob", "Cleanup", "format", NULL);
		exit(EXIT_SUCCESS);
	}

	if (!strcmp(s, "nob"))
		return conf->extra.build_nob = true, 0;
	if (!strcmp(s, "Cleanup"))
		return conf->extra.tools_Cleanup = true, 0;
	if (!strcmp(s, "format"))
		return conf->extra.tools_format = true, 0;
	fprintf(stderr, "Option '%s' is not valid. See %s --extras=list\n", s,
		PROGRAM);
	return 1;
}

static int parse_arguments(manifest_t *conf, int argc, char **argv)
{
	int opt, option_index;

	// clang-format off
	static struct option long_opts[] = {
		{ "git", no_argument, 0, 'g' },
		{ "no-git", no_argument, 0, 'G' },
		{ "lib", no_argument, 0, 'L' },
		{ "autotools", no_argument, 0, 'a' },
		{ "cmake", no_argument, 0, 'c' },
		{ "make", no_argument, 0, 'm' },
		{ "bare", no_argument, 0, 'B' },
		{ "flat", no_argument, 0, 'f' },
		{ "author", required_argument, 0, 'A' },
		{ "extras", required_argument, 0, 0 },
		{ 0, 0, 0, 0 } };
	// clang-format on

	while ((opt = getopt_long(argc, argv, "gGLbacmBfAl:E", long_opts,
				  &option_index)) != -1) {
		if (opt == 0 &&
		    strcmp(long_opts[option_index].name, "extras") == 0) {
			int err;
			char *arg = optarg;
			char *token = strtok(arg, ",");
			while (token) {
				err = parse_extras_token(conf, token);
				if (err)
					exit(err);
				token = strtok(NULL, ",");
			}
		}
		switch (opt) {
		case 'g':
			conf->git = true;
			break;
		case 'G':
			conf->git = false;
			break;
		case 'L':
			conf->lib = true;
			break;
		case 'a':
			conf->build = AUTOTOOLS;
			break;
		case 'c':
			conf->build = CMAKE;
			break;
		case 'm':
			conf->build = MAKE;
			break;
		case 'B':
			conf->build = BARE;
			break;
		case 'f':
			conf->flat = true;
			break;
		case 'A':
			conf->author = optarg;
			break;
		case 'l':
			if (!strcmp(optarg, "list")) {
				print_lines("MIT", "BSD", "GPL", "UNL", NULL);
				exit(EXIT_SUCCESS);
			}
			conf->licence = TOlicence(optarg);
			break;
		case 'E':
			conf->open_editor = true;
			conf->editor = getenv("EDITOR");
			break;
		default:
			return 1;
		}
	}

	if (optind >= argc) {
		return HELP_REQUESTED;
	}
	conf->project = str_dup(argv[optind]);

	return 0;
}

int get_name(char **output)
{
	FILE *fp;
	char buf[256];
	char *res = NULL;
	struct passwd *pw;

	fp = popen("git config --get user.name", "r");
	if (fp) {
		if (fgets(buf, sizeof buf, fp)) {
			size_t len = strlen(buf);
			if (len > 0 && buf[len - 1] == '\n')
				buf[len - 1] = '\0';
			res = strdup(buf);
		}
		pclose(fp);
	}

	if (!res) {
		pw = getpwuid(getuid());
		if (!pw || !pw->pw_name)
			return -1;
		res = strdup(pw->pw_name);
	}

	if (!res)
		return -1;

	*output = res;
	return 0;
}

int main(int argc, char **argv)
{
	int status;
	manifest_t manifest = {
		.project = "Project",
		.author = "author",
		.editor = "vim",

		.licence = UNL,

		.git = true,
		.build = MAKE,
		.flat = false,
		.open_editor = false,
		.lib = false,

		.extra.build_nob = false,
		.extra.tools_format = false,
		.extra.tools_Cleanup = false,
	};

	status = parse_standard_options(usage, argc, argv);
	if (status != 0 && status != HELP_REQUESTED)
		return fprintf(stderr, "error: %s\n", strerror(status)),
		       EXIT_FAILURE;

	status = parse_arguments(&manifest, argc, argv);
	if (status == HELP_REQUESTED) {
		usage(0);
	}

	if (status) {
		return EXIT_FAILURE;
	}

	get_name(&manifest.author);
	status = create_project(manifest);

	if (status) {
		fprintf(stderr, "%s\n", strerror(status));
		return EXIT_FAILURE;
	}

	char *cwd;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL) {
		perror("getcwd");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Created %s at\n %s\n", manifest.project, cwd);

	free(cwd);

	return EXIT_SUCCESS;
}
