/* Copyright (C) vx-clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

// Usage: yait [OPTION]... <PROJECT>

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../config.h"
#include "../lib/err.h"
#include "../lib/fs.h"
#include "../lib/xmem.h"
#include "../lib/proginfo.h"
#include "../lib/textc.h"

typedef enum { MIT, GPL, BSD, UNL } licence_t;

static const struct option longopts[] = {
	{ "author", required_argument, 0, 'a' },
	{ "licence", required_argument, 0, 'l' },
	{ "quiet", no_argument, 0, 'q' },
	{ "force", no_argument, 0, 'f' },
	{ 0, 0, 0, 0 }
};

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
	char *package;
	set_prog_name(argv[0]);

	exit_status = EXIT_SUCCESS;
	bool quiet = false;
	bool force = false;
	bool editor = false;
	const char *author = get_name();
	licence_t licence;

	parse_standard_options(argc, argv, print_help, print_version);

	while ((optc = getopt_long(argc, argv, "a:l:Eqf", longopts, NULL)) !=
	       -1)
		switch (optc) {
		case 'l':
			if (!strcmp(optarg, "list")) {
				puts("BSD\nGPL\nMIT\nUNL");
				exit(EXIT_SUCCESS);
			}
			if (!strcmp(optarg, "GPL"))
				licence = GPL;
			else if (!strcmp(optarg, "MIT"))
				licence = MIT;
			else if (!strcmp(optarg, "BSD"))
				licence = BSD;
			else if (!strcmp(optarg, "UNL"))
				licence = UNL;
			else {
				puts("BSD\nGPL\nMIT\nUNL");
				exit(EXIT_FAILURE);
			}
			break;
		case 'E':
			editor = true;
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
	if (lose) {
		emit_try_help();
	}

	if (optind >= argc) {
		fatalf("no project name provided");
	}

	if (optind + 1 < argc) {
		errorf("extra operand: %s", argv[optind + 1]);
		emit_try_help();
	}

	package = str_dup(argv[optind]);

	size_t len = strlen(package);
	char *pdir = xmalloc(len + 2);
	memcpy(pdir, package, len);
	pdir[len] = '/';
	pdir[len + 1] = '\0';

	fs_write("README", "\
This is the README for the GCK %s distribution.\n\
%s does a thing.\n\
\n\
  Copyright (C) %d GCK.\n\
\n\
  Copying and distribution of this file, with or without modifications\n\
  are permitted in any medium without royalty provided the copyright\n\
  notice and this notice are preserved.\n\
\n\
See the files ./INSTALL* for building and installation instructions.\n\
\n\
Bug reports:\n\
 Please include enough information for the maintainers to reproduce the\n\
 problem. Generally speaking, that means:\n\
- the contents of any input files necessary to reproduce the bug\n\
  and command line invocations of the program(s) involved (crucial!).\n\
- a description of the problem and any samples of the erroneous output.\n\
- the version number of the program(s) involved (use --version).\n\
- hardware, operating system, and compiler versions (uname -a).\n\
- unusual options you gave to configure, if any (see config.mak).\n\
- anything else that you think would be helpful.\n\
\n\
See README-dev for information on the development environment -- any\n\
interested parties are welcome. If you're a programmer and wish to\n\
contribute, this should get you started. If you're not a programmer,\n\
your help in writing test cases, checking documentation against the\n\
implementation, etc., would still be very much appreciated.\n\
\n\
GCK %s is free software. See the file COPYING for copying conditions.\n\
\n",
		 package, package, YEAR, package);

	fs_write("INSTALL", "\
Installation Instructions\n\
*************************\n\
\n\
Copyright (C) %d GCK.\n\
\n\
   Copying and distribution of this file, with or without modification,\n\
are permitted in any medium without royalty provided the copyright\n\
notice and this notice are preserved.  This file is offered as-is,\n\
without warranty of any kind.\n\
\n\
Basic Installation\n\
==================\n\
\n\
   Briefly, the shell command `./configure && make && make install` should\n\
configure, build, and install this package. The following more-detailed\n\
instruction are generic; see the `README` file for instructions specific to\n\
this package.\n\
\n\
   The `configure` shell script attempts to guess correct values for\n\
various system-dependent variables used during compilation. It uses\n\
those values within a `Makefile` to build for that POSIX system as\n\
defined by `config.mak` which was generated by `configure`.\n\
\n\
Compilers and Options\n\
=====================\n\
\n\
  Some systems require unusal options for compilation or linking that\n\
the `configure` script does not know about. If you run into an issue\n\
run `./configure --help` to figure out what you can do to fix the\n\
behavoir.\n\
\n\
Installation Names\n\
==================\n\
\n\
  By default, `make install` installs the package's command under\n\
`/usr/local/bin`. You can specify an installation prefix other than `/usr/local/`\n\
by giving `configure` the option `--prefix=PREFIX` to `configure`, the package uses\n\
PREFIX as the prefix for installation programs and libraries.\n\
Documentation and other data files still use the regular prefix.\n\
\n\
`configure` Invokation\n\
======================\n\
\n\
  `configure` recongizes the following options to control its operations.\n\
\n\
  `--help`\n\
     Prints a summary of all the options to `configure`, and exits.\n\
  `--prefix=PREFIX`\n\
     Sets the installation prefix.\n\
  `CFLAGS`\n\
    Sets the flags used during compilation.\n\
\n\
`configure` also accepts some other options. Run `configure --help` for more\n\
details\n\
",
		 YEAR);

	fs_write("AUTHORS", "\
Authors of GCK yait.\n\
\n\
  Copyright (C) 2025 GCK.\n\
\n\
  Copying and distribution of this file, with or without modification,\n\
  are permitted in any medium without royalty provided the copyright\n\
  notice and this notice are preserved.\n\
\n\
Also see the THANKS files.\n\
\n\
%s\n\
",
		 author);

	fs_write("THANKS", "\
Additional contributors to GCK %s.\n\
\n\
  Copyright (C) %d GCK.\n\
\n\
  Copying and distribution of this file, with or without modification,\n\
  are permitted in any medium without royalty provided the copyright\n\
  notice and this notice are preserved.\n\
\n\
Thanks to:\n\
\n\
    GCK yait for project initialization.\n\
\n\
See also the AUTHORS file.\n\
			",
		 package, YEAR);

	fs_write("config.h", "\
#ifndef CONFIG_H\n\
#define CONFIG_H\n\
\n\
/* Program information */\n\
#define PROGRAM \"%s\"\n\
#define AUTHORS \"GCK\"\n\
#define VERSION \"beta\"\n\
#define YEAR %d\n\
\n\
#endif\n\
			",
		 package, YEAR);

	fs_write("configure", "\
#!/bin/sh\n\
\n\
usage() {\n\
cat <<EOF\n\
Usage: $0 [OPTION]... [VAR=VALUE]...\n\
\n\
To assign environment variables (e.g., CC, CFLAGS...), specify them as\n\
VAR=VALUE.\n\
\n\
CC              C compiler command [detected]\n\
CFLAGS          C compiler flags [-g, ...]\n\
LDFLAGS         C linker flags\n\
\n\
--prefix=<path> Set the install path\n\
--debug         Flags for debug build, overrides CFLAGS\n\
\n\
EOF\n\
exit 0\n\
}\n\
\n\
cmdexists() { type \"$1\" >/dev/null 2>&1 ; }\n\
trycc() { [ -z \"$CC\" ] && cmdexists \"$1\" && CC=$1 ; }\n\
\n\
prefix=/usr/local\n\
CFLAGS=\"-std=c23\"\n\
LDFLAGS=\n\
CC=\n\
\n\
printf \"checking for C compiler... \"\n\
trycc gcc\n\
trycc clang\n\
trycc cc\n\
trycc icx\n\
printf \"%s\n\" \"$CC\"\n\
\n\
DEBUG=false\n\
for arg; do\n\
case \"$arg\" in\n\
--help|-h) usage ;;\n\
--prefix=*) prefix=${arg#*=} ;;\n\
--debug) DEBUG=true ;;\n\
CFLAGS=*) CFLAGS=${arg#*=} ;;\n\
LDFLAGS=*) LDFLAGS=${arg#*=} ;;\n\
CC=*) CC=${arg#*=} ;;\n\
*) printf \"Unrecognized option %s\n\" \"$arg\" ;;\n\
esac\n\
done\n\
\n\
printf \"checking whether C compiler works... \"\n\
tmpc=\"$(mktemp -d)/test.c\"\n\
echo \"typedef int x;\" > \"$tmpc\"\n\
if output=$($CC $CFLAGS -c -o /dev/null \"$tmpc\" 2>&1); then\n\
printf \"yes\n\"\n\
else\n\
printf \"no; %%s\n\" \"$output\"\n\
exit 1\n\
fi\n\
\n\
GDEBUGCFLAGS=\"-std=c23 -O0 -g3 -Wall -Wextra -Wpedantic -Werror -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wconversion -Wsign-conversion -Wcast-qual -Wcast-align=strict -Wpointer-arith -Wstrict-overflow=5 -Wstrict-aliasing=2 -Wundef -Wunreachable-code -Wswitch-enum -fanalyzer -fsanitize=undefined,address -fstack-protector-strong -D_FORTIFY_SOURCE=3\"\n\
CDEBUGCFLAGS=\"-std=gnu2x -O0 -g3 -Wall -Wextra -Wpedantic -Werror -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wconversion -Wsign-conversion -Wcast-qual -Wcast-align=strict -Wpointer-arith -Wstrict-overflow=5 -Wstrict-aliasing=2 -Wundef -Wunreachable-code -Wswitch-enum -fanalyzer -fsanitize=undefined,address -fstack-protector-strong -D_FORTIFY_SOURCE=3\"\n\
\n\
if [ -z \"$DEBUG\" ]; then\n\
case \"$CC\" in\n\
gcc) CFLAGS=\"$GDEBUGFLAGS\";;\n\
clang) CFLAGS=\"$CDEBUGFLAGS\";;\n\
*) ;;\n\
esac\n\
else\n\
case \"$CC\" in\n\
gcc) ;;\n\
clang) ;;\n\
*) ;;\n\
esac\n\
fi\n\
\n\
case \"$OSTYPE\" in\n\
cygwin|msys) \n\
echo \"enabling windows specific flags\"\n\
CFLAGS=\"-v $CFLAGS\"\n\
;;\n\
esac\n\
\n\
printf \"creating config.mak... \"\n\
{\n\
printf \"PREFIX=%%s\n\" \"$prefix\"\n\
printf \"CFLAGS=%%s\n\" \"$CFLAGS\"\n\
printf \"LDFLAGS=%%s\n\" \"$LDFLAGS\"\n\
printf \"CC=%%s\n\" \"$CC\"\n\
} > config.mak\n\
printf \"done\n\"\n\
");

	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL) {
		fatalfa(errno);
	}

	if (!quiet) {
		fprintf(stderr, "Created %s at\n %s\n", package, cwd);
	}

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

static void print_version()
{
	printf("%s %s %d\n", prog_name, VERSION, COMMIT);

	printf("Copyright (C) %d GCK.\n", YEAR);

	puts("This is free software: you are free to change and redistribute it.");
	puts("There is NO WARRNTY, to the extent permitted by law.");
	exit(exit_status);
}
