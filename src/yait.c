/*
 *   yait vbeta - An opionated C project creator
 *
 *   FEATURES:
 *       - Generate complete build systems: Autotools-like Makefile
 *       - Generate complete base system, ready to compile
 -       - Generate preconfigured gcklib, ready to import
 *
 *   LIMITATIONS:
 *       - Platform: Only builds for POSIX systems
 *
 *   COMPILATION (Linux - POSIX):
 *       ./configure && make
 *
 *
 *   LICENSE: BSD-3-Clause
 *
 *   Copyright (c) 2025 GCK
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>
#include <errno.h>
#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "licence.h"
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
	return str_dup("author");
}

static int get_year()
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	// The tm_year member stores years since 1900, so add 1900 to get the actual year
	return t->tm_year + 1900;
}

int main(int argc, char **argv)
{
	int optc;
	int lose = 0;
	char *package;
	bool quiet = false;
	bool force = false;
	bool editor = false;
	bool shell = false;
	char *author = get_name();
	exit_status = EXIT_SUCCESS;
	int year = get_year();
	licence_t licence = BSD;
	set_prog_name(argv[0]);

	parse_standard_options(argc, argv, print_help, print_version);

	while ((optc = getopt_long(argc, argv, "a:l:EqfS", longopts, NULL)) !=
	       -1)
		switch (optc) {
		case 'a':
			if (optarg) {
				if (author)
					free(author);
				author = str_dup(optarg);
			}
			break;
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
		case 'S':
			shell = true;
			break;
		default:
			lose = 1;
		}
	if (lose) {
		emit_try_help();
	}

	if (optind >= argc) {
		fatalf("no input name");
	}

	if (optind + 1 < argc) {
		errorf("extra operand: %s", argv[optind + 1]);
		emit_try_help();
	}

	package = str_dup(argv[optind]);

	if (shell) {
		fs_write(package, "\
#!/bin/sh\n\
\n\
# Usage: $0 [options]...\n\
\n\
prog_name=$(basename $0)\n\
tool_version=\"beta\"\n\
year=%d\n\
\n\
fatal() {\n\
	echo \"fatal: $*\" >&2\n\
	exit 1\n\
}\n\
\n\
run() {\n\
	\"$@\" || fatal \"could not run: $*\"\n\
}\n\
\n\
print_help() {\n\
    cat <<EOF\n\
Usage: $prog_name [options]...\n\
\n\
      --help     print this help and exit.\n\
      --version  print version information.\n\
EOF\n\
}\n\
\n\
print_version() {\n\
    cat <<EOF\n\
$prog_name $tool_version $(git rev-list --count --all 2>/dev/null || echo 0)\n\
Copyright (C) $year %s.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
EOF\n\
}\n\
\n\
while [ $# -gt 0 ]; do\n\
    case \"$1\" in\n\
        --help) print_help; exit 0 ;;\n\
        --version) print_version; exit 0 ;;\n\
	*) fatal \"Not implemented yet\" ;;\n\
    esac\n\
    shift\n\
done\n\
",
			 year, author);
		struct stat st;

		if (stat(package, &st) != 0) {
			fatalfa(errno);
		}

		mode_t mode = st.st_mode | S_IXUSR;

		if (chmod(package, mode) != 0) {
			fatalfa(errno);
		}
		return exit_status;
	}

	size_t len = strlen(package);
	char *pdir = xmalloc(len + 2);
	memcpy(pdir, package, len);
	pdir[len] = '/';
	pdir[len + 1] = '\0';

	if (fs_new(pdir) == -1 && errno != EEXIST)
		fatalfa(errno);
	if (chdir(pdir))
		fatalfa(errno);

	fs_write("doc/version.texi", "\
@set UPDATED %s\
@set UPDATED-MONTH %s\
@set EDITION 1\
@set VERSION alpha\
",
		 "1 January 1970", "January 2025");

	char path[BUFSIZ];
	snprintf(path, sizeof(path), "doc/%s.texi", package);
	fs_write(path, "\
\\input texinfo @c -*-texinfo-*-\n\
@c %**start of header\n\
@setfilename foo.info\n\
@include version.texi\n\
@settitle %s foo @value{VERSION}\n\
\n\
@defcodeindex op\n\
@syncodeindex op cp\n\
@c %**end of header\n\
\n\
@copying\n\
This manual is for %s foo (version @value{VERSION}, @value{UPDATED}),\n\
a simple program for demonstrating Texinfo documentation.\n\
\n\
Copyright @copyright{} 2025 %s.\n\
\n\
@quotation\n\
Copying and distribution of this file, with or without modification,\n\
are permitted in any medium without royalty provided the copyright\n\
notice and this notice are preserved.\n\
@end quotation\n\
@end copying\n\
\n\
@titlepage\n\
@title %s foo\n\
@subtitle for version @value{VERSION}, @value{UPDATED}\n\
@page\n\
@vskip 0pt plus 1filll\n\
@insertcopying\n\
@end titlepage\n\
\n\
@contents\n\
\n\
@ifnottex\n\
@node Top\n\
@top %s foo\n\
\n\
This manual is for %s foo (version @value{VERSION}, @value{UPDATED}),\n\
a simple program for demonstrating Texinfo documentation.\n\
@end ifnottex\n\
\n\
@menu\n\
* Overview::           General overview and purpose.\n\
* Sample output::      Example usage and output.\n\
* Invoking foo::       How to run @command{foo}.\n\
* Reporting bugs::     Sending bug reports and suggestions.\n\
* Concept index::      Index of concepts.\n\
@end menu\n\
\n\
\n\
@node Overview\n\
@chapter Overview\n\
\n\
@cindex overview\n\
@cindex purpose\n\
\n\
The %s @command{foo} program serves as a minimal example of a %s utility.\n\
Its purpose is to show how to build and document small command-line tools\n\
that follow GNU-style conventions.\n\
\n\
@itemize @bullet\n\
@item\n\
Implements clean command-line option parsing using GNU-style long options.\n\
@item\n\
Provides simple, predictable behavior for testing build systems.\n\
@item\n\
Demonstrates how to write Texinfo manuals for %s programs.\n\
@end itemize\n\
\n\
@cindex implementation\n\
%s foo is implemented in C and follows the GNU coding and maintainer standards.\n\
It uses Autotools for configuration and build setup, and Texinfo for documentation.\n\
\n\
\n\
@node Sample output\n\
@chapter Sample output\n\
\n\
@cindex examples\n\
@cindex sample output\n\
\n\
Here are some examples of running %s foo:\n\
\n\
@example\n\
$ foo\n\
foo: hello, world!\n\
@end example\n\
\n\
@example\n\
$ foo --message=\"%s rules!\"\n\
foo: %s rules!\n\
@end example\n\
\n\
@example\n\
$ foo --version\n\
%s foo @value{VERSION}\n\
@end example\n\
\n\
\n\
@node Invoking foo\n\
@chapter Invoking @command{foo}\n\
\n\
@cindex invoking\n\
@cindex usage\n\
@cindex options\n\
\n\
The general form for running @command{foo} is:\n\
\n\
@example\n\
foo @var{option} @dots{}\n\
@end example\n\
\n\
With no options, @command{foo} prints a default message.\n\
\n\
@command{foo} supports the following options:\n\
\n\
@table @option\n\
@item --message=@var{text}\n\
@itemx -m @var{text}\n\
@opindex --message\n\
@opindex -m\n\
Print @var{text} instead of the default greeting.\n\
\n\
@item --help\n\
@itemx -h\n\
@opindex --help\n\
@opindex -h\n\
Display help text and exit successfully.\n\
\n\
@item --version\n\
@itemx -v\n\
@opindex --version\n\
@opindex -v\n\
Print the version number and licensing information, then exit.\n\
@end table\n\
\n\
\n\
@node Reporting bugs\n\
@chapter Reporting bugs\n\
\n\
@cindex bugs\n\
@cindex reporting\n\
@cindex contact\n\
\n\
When reporting bugs, include:\n\
@itemize @bullet\n\
@item The output of @samp{foo --version}.\n\
@item Your operating system and compiler version.\n\
@item The exact command line used.\n\
@item Any relevant output or error messages.\n\
@end itemize\n\
\n\
Patches are welcome, preferably made with @samp{diff -u} and including\n\
a @file{ChangeLog} entry.\n\
\n\
\n\
@node GNU Free Documentation License\n\
@appendix GNU Free Documentation License\n\
\n\
@include fdl.texi\n\
\n\
\n\
@node Concept index\n\
@unnumbered Concept index\n\
\n\
@printindex cp\n\
\n\
@bye\
",
		 author, author, author, author, author, author, author, author,
		 author, author, author, author, author, author);

	// TODO(vx-clutch): Why dosn't this write the source?
	snprintf(path, sizeof(path), "src/%s.c", package);
	fs_write(path, "\
/* Copyright (C) %s\n\
 *\n\
 * This file is part of %s\n\
 *\n\
 * This project and file is licenced under the BSD-3-Clause licence.\n\
 * <https://opensource.org/licence/bsd-3-clause>\n\
 */\n\
\n\
// Usage: %s [OPTION]...\n\
\n\
#include <stdlib.h>\n\
#include <stdio.h>\n\
\n\
#include <config.h>\n\
\n\
#include \"../lib/proginfo.h\"\n\
\n\
static int exit_status;\n\
\n\
static void print_help();\n\
static void print_version();\n\
\n\
int main(int argc, char **argv)\n\
{\n\
	set_prog_name(argv[0]);\n\
	\n\
	exit_status = EXIT_SUCCESS;\n\
\n\
	return exit_status;\n\
}\n\
\n\
void print_help()\n\
{\n\
	printf(\"Usage: %%s [OPTION]...\\n\", PROGRAM);\n\
	fputs(\"\\
%s does a thing.\\n\",\n\
		 stdout);\n\
	\n puts(\"\");\n\
	fputs(\"\\
	  --help               display this help and exit\\n\\
	  --version            display version information and exit\\n\",\n\
	      stdout);\n\
	puts(\"\");\n\
	fputs(\"\\
	  --option             Does an awesome thing\\n\\\",\n\
	  stdout);\n\
	exit(exit_status);\n\
}\n\
\n\
void print_version()\n\
{\n\
	printf(\"%%s %%s %%d\\n\", prog_name, VERSION, COMMIT);\n\
	\n\
	printf(\"Copyright (C) %%d %s.\\n\", YEAR);\n\
	\n\
	puts(\"This is free software: you are free to change and redistribute it.\");\n\
	puts(\"There is NO WARRANTY, to the extent permitted by law.\");\n\
	exit(exit_status);\n\
}\
",
		 author, package, package, package, author);

	fs_write("tools/Cleanup", "\
#!/bin/sh\n\
# Usage: ./Cleanup\n\
\n\
fatal() {\n\
    echo \"fatal: $*\" >&2\n\
    exit 1\n\
}\n\
\n\
run() {\n\
    \"$@\" || fatal \"could not run: $*\"\n\
}\n\
\n\
[ -d \"./git\" ] && fatal \"must be run from parent directory\"\n\
\n\
run sh ./tools/format\n\
run rm -rf .cache\n\
run rm -f compile_commands.json\n\
run make distclean\n\
\n\
echo \"done.\"\
");
	fs_write("tools/format", "\
#!/bin/sh\n\
\n\
# Usage ./format\n\
\n\
find . -name \"*.c\" -exec clang-format -i --verbose {} \\;\n\
find . -name \"*.h\" -exec clang-format -i --verbose {} \\;\
");

	fs_write(".clang-format", "\
---\n\
AccessModifierOffset: -4\n\
AlignAfterOpenBracket: Align\n\
AlignConsecutiveAssignments: false\n\
AlignConsecutiveDeclarations: false\n\
AlignEscapedNewlines: Left\n\
AlignOperands: true\n\
AlignTrailingComments: false\n\
AllowAllParametersOfDeclarationOnNextLine: false\n\
AllowShortBlocksOnASingleLine: false\n\
AllowShortCaseLabelsOnASingleLine: false\n\
AllowShortFunctionsOnASingleLine: None\n\
AllowShortIfStatementsOnASingleLine: false\n\
AllowShortLoopsOnASingleLine: false\n\
AlwaysBreakAfterDefinitionReturnType: None\n\
AlwaysBreakAfterReturnType: None\n\
AlwaysBreakBeforeMultilineStrings: false\n\
AlwaysBreakTemplateDeclarations: false\n\
BinPackArguments: true\n\
BinPackParameters: true\n\
BraceWrapping:\n\
  AfterClass: false\n\
  AfterControlStatement: false\n\
  AfterEnum: false\n\
  AfterFunction: true\n\
  AfterNamespace: true\n\
  AfterObjCDeclaration: false\n\
  AfterStruct: false\n\
  AfterUnion: false\n\
  AfterExternBlock: false\n\
  BeforeCatch: false\n\
  BeforeElse: false\n\
  IndentBraces: false\n\
  SplitEmptyFunction: true\n\
  SplitEmptyRecord: true\n\
  SplitEmptyNamespace: true\n\
BreakBeforeBinaryOperators: None\n\
BreakBeforeBraces: Custom\n\
BreakBeforeInheritanceComma: false\n\
BreakBeforeTernaryOperators: false\n\
BreakConstructorInitializersBeforeComma: false\n\
BreakConstructorInitializers: BeforeComma\n\
BreakAfterJavaFieldAnnotations: false\n\
BreakStringLiterals: false\n\
ColumnLimit: 80\n\
CommentPragmas: '^ IWYU pragma:'\n\
CompactNamespaces: false\n\
ConstructorInitializerAllOnOneLineOrOnePerLine: false\n\
ConstructorInitializerIndentWidth: 8\n\
ContinuationIndentWidth: 8\n\
Cpp11BracedListStyle: false\n\
DerivePointerAlignment: false\n\
DisableFormat: false\n\
ExperimentalAutoDetectBinPacking: false\n\
FixNamespaceComments: false\n\
\n\
IncludeBlocks: Preserve\n\
IncludeCategories:\n\
  - Regex: '.*'\n\
    Priority: 1\n\
IncludeIsMainRegex: '(Test)?$'\n\
IndentCaseLabels: false\n\
IndentGotoLabels: false\n\
IndentPPDirectives: None\n\
IndentWidth: 8\n\
IndentWrappedFunctionNames: false\n\
JavaScriptQuotes: Leave\n\
JavaScriptWrapImports: true\n\
KeepEmptyLinesAtTheStartOfBlocks: false\n\
MacroBlockBegin: ''\n\
MacroBlockEnd: ''\n\
MaxEmptyLinesToKeep: 1\n\
NamespaceIndentation: None\n\
ObjCBinPackProtocolList: Auto\n\
ObjCBlockIndentWidth: 8\n\
ObjCSpaceAfterProperty: true\n\
ObjCSpaceBeforeProtocolList: true\n\
\n\
PenaltyBreakAssignment: 10\n\
PenaltyBreakBeforeFirstCallParameter: 30\n\
PenaltyBreakComment: 10\n\
PenaltyBreakFirstLessLess: 0\n\
PenaltyBreakString: 10\n\
PenaltyExcessCharacter: 100\n\
PenaltyReturnTypeOnItsOwnLine: 60\n\
\n\
PointerAlignment: Right\n\
ReflowComments: false\n\
SortIncludes: false\n\
SortUsingDeclarations: false\n\
SpaceAfterCStyleCast: false\n\
SpaceAfterTemplateKeyword: true\n\
SpaceBeforeAssignmentOperators: true\n\
SpaceBeforeCtorInitializerColon: true\n\
SpaceBeforeInheritanceColon: true\n\
SpaceBeforeParens: ControlStatementsExceptForEachMacros\n\
SpaceBeforeRangeBasedForLoopColon: true\n\
SpaceInEmptyParentheses: false\n\
SpacesBeforeTrailingComments: 1\n\
SpacesInAngles: false\n\
SpacesInContainerLiterals: false\n\
SpacesInCStyleCastParentheses: false\n\
SpacesInParentheses: false\n\
SpacesInSquareBrackets: false\n\
Standard: Cpp03\n\
TabWidth: 8\n\
UseTab: Always\n\
...\
");

	fs_write(".clangd", "\
CompileFlags:\n\
  Add: [-x, c, -std=c23, -Ilib, -I.]\n\
\n\
Diagnostics:\n\
  ClangTidy:\n\
    Add: [clang-diagnostic-*]\n\
    Remove: []\
");

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
		 package, package, year, package);

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
		 year);

	fs_write("AUTHORS", "\
Authors of %s %s.\n\
\n\
  Copyright (C) %d %s.\n\
\n\
  Copying and distribution of this file, with or without modification,\n\
  are permitted in any medium without royalty provided the copyright\n\
  notice and this notice are preserved.\n\
\n\
Also see the THANKS files.\n\
\n\
%s\n\
",
		 author, package, year, package, author);

	fs_write("THANKS", "\
Additional contributors to %s %s.\n\
\n\
  Copyright (C) %d %s.\n\
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
		 author, package, year, author);

	fs_write("config.h", "\
#ifndef CONFIG_H\n\
#define CONFIG_H\n\
\n\
/* Program information */\n\
#define PROGRAM \"%s\"\n\
#define AUTHORS \"%s\"\n\
#define VERSION \"beta\"\n\
#define YEAR %d\n\
\n\
#endif\
",
		 package, author, year);

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
printf \"%%s\n\" \"$CC\"\n\
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
*) printf \"Unrecognized option %%s\n\" \"$arg\" ;;\n\
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
if [ \"$DEBUG\" = \"false\" ]; then\n\
case \"$CC\" in\n\
gcc) CFLAGS=\"$GDEBUGCFLAGS\";;\n\
clang) CFLAGS=\"$CDEBUGCFLAGS\";;\n\
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
printf \"done\n\"\
");

	fs_write("Makefile", "\
PACKAGE := %s\n\
\n\
SRCS := $(wildcard src/*.c) $(wildcard lib/*.c)\n\
OBJS := $(patsubst src/%%.c,build/obj/%%.o,$(SRCS))\n\
\n\
BIN := bin/$(PACKAGE)\n\
\n\
COMMIT := $(shell git rev-list --count --all)\n\
FLAGS := -I. -DCOMMIT=$(COMMIT)\n\
\n\
VERSION := $(shell git describe --tags --always --dirty)\n\
TARBALL := $(PACKAGE)-$(VERSION).tar.gz\n\
RELEASE_FILES := doc src lib COPYING AUTHORS README yait.1 INSTALL Makefile configure config.h\n\
\n\
-include config.mak\n\
\n\
ifeq ($(wildcard config.mak),)\n\
all:\n\
	@echo \"File config.mak not found, run configure \"\n\
	@exit 1\n\
else\n\
\n\
all: build $(BIN)\n\
\n\
build:\n\
	mkdir -p bin\n\
	mkdir -p build/obj\n\
\n\
build/obj/%%.o: $(SRCS) config.mak\n\
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@\n\
\n\
$(BIN): $(OBJS) \n\
	$(CC) $(FLAGS) $(CFLAGS) $^ -o $@\n\
\n\
endif\n\
\n\
install: $(BIN)\n\
	cp $(BIN) $(PREFIX)\n\
\n\
uninstall:\n\
	$(RM) $(PREFIX)$(PACKAGE)\n\
\n\
clean:\n\
	$(RM) $(BIN)\n\
	$(RM) -r build\n\
\n\
distclean: clean\n\
	$(RM) config.mak\n\
	$(RM) $(TARBALL)\n\
\n\
release: clean all\n\
	tar -czf $(TARBALL) $(RELEASE_FILES)\n\
\n\
.PHONY: all clean distclean install uninstall build release\
",
		 package);

	fs_write("TODO", "\
%s %s- TODO\n\
\n\
Todo:\n\
\n\
  * Generate %s.1 with help2man.\n\
\n\
end of file TODO\
",
		 author, package, package);

	switch (licence) {
	case BSD:
		fs_write("COPYING", fBSD, year, author);
		break;
	case MIT:
		fs_write("COPYING", fMIT, year, author);
		break;
	case GPL:
		fs_write("COPYING", fGPL);
		break;
	case UNL:
		fs_write("COPYING", fUNL);
		break;
	default:
		fatalf("illegal state");
	}

	struct stat st;

	if (stat("configure", &st) != 0) {
		fatalfa(errno);
	}

	mode_t mode = st.st_mode | S_IXUSR;

	if (chmod("configure", mode) != 0) {
		fatalfa(errno);
	}
	if (chmod("tools/format", mode) != 0) {
		fatalfa(errno);
	}
	if (chmod("tools/Cleanup", mode) != 0) {
		fatalfa(errno);
	}

	return exit_status;
}

static void print_help()
{
	printf("Usage: %s [OPTION]... [project-name]...\n", PROGRAM);
	fputs("\
Generates an opinionated C project.\n",
	      stdout);
	puts("");
	fputs("\
      --help                  display this help and exit\n\
      --version               display version information and exit\n",
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
	puts("There is NO WARRANTY, to the extent permitted by law.");
	exit(exit_status);
}

/* end of file yait.c */
