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
#include <sys/wait.h>

#include "../lib/proginfo.h"
#include "../lib/err.h"
#include "../lib/str_dup.h"
#include "../include/yait.h"
#include "../config.h"
#include "name.h"

static const struct option longopts[] = {
        { "author", required_argument, 0, 'a' },
        { "licence", required_argument, 0, 'l' },
        { "quiet", no_argument, 0, 'q' },
        { "force", no_argument, 0, 'f' },
        { 0, 0, 0, 0 } };

static int exit_status;

static void print_help();
static void print_version();

static char *get_name()
{
	int fds[2];
	if (pipe(fds) == -1)
		goto sysuser;

	pid_t pid = fork();
	if (pid == -1) {
		close(fds[0]);
		close(fds[1]);
		goto sysuser;
	}

	if (pid == 0) {
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
		execlp("git", "git", "config", "--get", "user.name",
		       (char *)NULL);
		_exit(127);
	}

	close(fds[1]);
	char buf[256];
	ssize_t n = read(fds[0], buf, sizeof buf - 1);
	close(fds[0]);
	int status;
	waitpid(pid, &status, 0);
	if (n > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		buf[n] = 0;
		buf[strcspn(buf, "\n")] = 0;
		return str_dup(buf);
	}

sysuser: {
	char *name = getlogin();
	if (name)
		return str_dup(name);
	struct passwd *pw = getpwuid(getuid());
	if (pw && pw->pw_name)
		return str_dup(pw->pw_name);
}
	return "author";
}

int main(int argc, char **argv)
{
	int optc;
	int lose = 0;
	set_prog_name(argv[0]);

	exit_status = EXIT_SUCCESS;
	bool quiet = false;
	bool force = false;

	manifest_t manifest = {
		.author = get_name(),
		.editor = NULL,
		.licence = BSD,
		.project = "Project",
	};

	parse_standard_options(argc, argv, print_help, print_version);

	while ((optc = getopt_long(argc, argv, "a:l:Eqf", longopts, NULL)) != -1)
		switch (optc) {
		case 'l':
			if (!strcmp(optarg, "list")) {
				puts("BSD\nGPL\nMIT");
				exit(EXIT_SUCCESS);
			}
			if (!strcmp(optarg, "GPL"))
				manifest.licence = GPL;
			else if (!strcmp(optarg, "MIT"))
				manifest.licence = MIT;
			else {
				puts("BSD\nGPL\nMIT");
				exit(EXIT_FAILURE);
			}
			break;
		case 'E':
			manifest.editor = getenv("EDITOR");
			break;
		case 'q':
			quiet = true;
			break;
		case 'f':
			force = true;
			break;
		default:
			lose = 1;
		}
	if (lose || optind < argc) {
		errorf("extra operand: %s", argv[optind]);
		usage();
	}

	char *cwd = getcwd(NULL, 0);
	if (!cwd) {
		fatalfa(errno);
	}
	fprintf(stderr, "Created %s at\n %s\n", manifest.project, cwd);
	free(cwd);

	return exit_status;
}

static void print_help()
{
	printf("Usage: %s [OPTION]... [project-name]...\n", PROGRAM);
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
      -E                      Open $EDITOR after project creation\n\
      -q, --quiet             Only print required messages\n\
      -f, --force             Overwrite existing files\n\
      --author=NAME           Set the program author (default git username|system username)\n\
      --licence=LICENCE       Set the program licence (default BSD)\n",
	      stdout);
	exit(exit_status);
}

/* Print version and copyright information.  */

static void print_version()
{
  printf("%s %s %d\n", prog_name, VERSION, COMMIT);
	
  printf("Copyright (C) %d GCK.\n", YEAR);

  puts("This is free software: you are free to change and redistribute it.");
  puts("There is NO WARRNTY, to the extent permitted by law.");
  exit(exit_status);
}
