#include "../config.h"
#include "../core/file.h"
#include "../core/print.h"
#include "../core/standard.h"
#include "format.h"
#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Constants for program behavior */
#define DEFAULT_USER_NAME "unknown"
#define DEFAULT_PROJECT_NAME "Project"
#define DEFAULT_LICENSE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true

/**
 * Create a new C project with the specified configuration
 * @param fmt Project configuration structure
 * @return 0 on success, non-zero on failure
 */
int create_project (format_t fmt);

/**
 * Print a formatted option line for help text
 * @param option The option name (left side)
 * @param description The option description (right side)
 */
#define print_option(option, description)                                     \
  printf ("  %-20s %-20s\n", option, description)

void
usage (int status)
{
  if (status != 0)
    fprintf (stderr, "Try 'yait --help' for more information.\n");
  else
    {
      printf ("Usage: yait [OPTION]... [PROJECT] (NAME)\n");
      printf ("Creates a C project with opinionated defaults.\nWhen only "
              "given first argument it will detect your name\n\n");
      printf ("Mandatory arguments to long options are mandatory for short "
              "options too\n");
      print_option ("-l, --license=NAME",
                    "Set license (gpl, mit, bsd) [default: gpl]");
      print_option ("--use-cpp", "Uses the CPP language instead of C");
      print_option ("--git", "Initialize git repository");
      print_option ("--GNU",
                    "Adds stand GNU argument parsing to your project");
      printf ("    --help\tdisplay the help text and exit\n");
      printf ("    --version\toutput version information and exit\n");
    }
}

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      printfn ("error: not enough arguments.");
      return 1;
    }
  int status = parse_standard_options (usage, argc, argv);
  if (status && status != HELP_REQUESTED)
    {
      printfn ("error: %s", strerror (status));
      return status;
    }
  format_t conf;
  conf.project = argv[1];
  if (argc > 2)
    conf.name = argv[2];
  else
    {
      struct passwd *pw = getpwuid (getuid ());
      if (pw && pw->pw_name)
        conf.name = pw->pw_name;
      else
        conf.name = DEFAULT_USER_NAME;
    }
  conf.git = DEFAULT_GIT_INIT;
  conf.clang_format = DEFAULT_CLANG_FORMAT;
  conf.licence = DEFAULT_LICENSE;

  int result = create_project (conf);
  return result;
}

/**
 * Create a new C project with the specified configuration
 * @param fmt Project configuration structure
 * @return 0 on success, non-zero on failure
 */
int
create_project (format_t fmt)
{
  int err = create_and_enter_directory (fmt.project);
  if (err)
    {
      printfn ("failed to create or enter directory: %s", strerror (err));
      return 1;
    }
  if (fmt.git)
    err = system ("git init --quiet");
  if (err)
    {
      printfn ("failed on git initialize: %s", strerror (err));
      return 1;
    }
  if (!fmt.name)
    fmt.name = DEFAULT_USER_NAME;
  create_file_with_content (
      "README",
      "%s ( concise description )\n\n"
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
      "eiusmod tempor\n"
      "incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
      "veniam, quis\n"
      "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
      "consequat.\n"
      "Duis aute irure dolor in reprehenderit in voluptate velit esse "
      "cillum dolore eu\n"
      "fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
      "proident, sunt in\n"
      "culpa qui officia deserunt mollit anim id est laborum.",
      fmt.project ? fmt.project : DEFAULT_PROJECT_NAME);
  create_file_with_content (
      "configure",
      "#!/bin/sh\n"
      "\n"
      "usage() {\n"
      "cat <<EOF\n"
      "Usage: $0 [OPTION]... [VAR=VALUE]...\n"
      "\n"
      "To assign environment variables (e.g., CC, CFLAGS...), specify them "
      "as\n"
      "VAR=VALUE.\n"
      "\n"
      "  CC      C compiler command [detected]\n"
      "  CFLAGS  C compiler flags [-g, ...]\n"
      "\n"
      "EOF\n"
      "exit 0\n"
      "}\n"
      "\n"
      "echo () { printf \"%%s\\n\" \"$*\" ; }\n"
      "cmdexists () { type \"$1\" >/dev/null 2>&1 ; }\n"
      "trycc () { test -z \"$CC\" && cmdexists \"$1\" && CC=$1 ; }\n"
      "\n"
      "prefix=/usr/bin/\n"
      "CFLAGS=\"-Wall -Wextra -O2\"\n"
      "LDFLAGS=\n"
      "CC=\n"
      "\n"
      "for arg ; do\n"
      "case \"$arg\" in\n"
      "--help|h) usage ;;\n"
      "CFLAGS=*) CFLAGS=${arg#*=} ;;\n"
      "LDFLAGS=*) LDFLAGS=${arg#*=} ;;\n"
      "esac\n"
      "done\n"
      "\n"
      "printf \"checking for C compiler... \"\n"
      "trycc gcc\n"
      "trycc cc\n"
      "trycc clang\n"
      "printf \"%%s\\n\" \"$CC\"\n"
      "\n"
      "printf \"checking weather C compiler works... \"\n"
      "status=\"fail\"\n"
      "tmpc=\"$(mktemp -d)/test.c\"\n"
      "echo \"typedef int x;\" > \"$tmpc\"\n"
      "if output=$($CC $CFLAGS -c -o /dev/null \"$tmpc\" 2>&1) ; then\n"
      "printf \"yes\\n\"\n"
      "else\n"
      "printf \"no; %%s\\n\" \"$output\"\n"
      "exit 1\n"
      "fi\n"
      "\n"
      "printf \"creating config.mak... \"\n"
      "printf \"PREFIX=%%s\\n\" \"$prefix\" > config.mak\n"
      "printf \"CFLAGS=%%s\\n\" \"$CFLAGS\" >> config.mak\n"
      "printf \"LDFLAGS=%%s\\n\" \"$LDFLAGS\" >> config.mak\n"
      "printf \"CC=%%s\\n\" \"$CC\" >> config.mak\n"
      "printf \"done\\n\"\n");
  // Create a safe uppercase version of the project name for Makefile variables
  char *mkfile_name = strdup (fmt.project);
  if (!mkfile_name)
    {
      printfn ("fatal: out of memory");
      return 1;
    }
  // Convert to uppercase safely, only for ASCII characters
  for (char *p = mkfile_name; *p; ++p)
    {
      if (*p >= 'a' && *p <= 'z')
        *p = *p - 'a' + 'A';
    }
  create_file_with_content (
      "Makefile",
      "prefix = /usr/bin\n\n"
      "%s_SRCS := $(wildcard *.c)\n"
      "%s_OBJS := $(patsubst %%.c,c-out/obj/%%.o,$(%s_SRCS))\n\n"
      "%s := c-out/bin/%s\n\n"
      "-include config.mak\n\n"
      "ifeq ($(wildcard config.mak),)\n"
      "all:\n"
      "\t@echo \"File config.mak not found, run configure\"\n"
      "\t@exit 1\n"
      "else\n\n"
      "all: build $(%s)\n\n"
      "build:\n"
      "\tmkdir -p c-out/bin\n"
      "\tmkdir -p c-out/obj\n\n"
      "c-out/obj/%%.o: %%.c\n"
      "\t$(CC) $(CFLAGS) -c $< -o $@\n\n"
      "$(%s): $(%s_OBJS)\n"
      "\t$(CC) $(CFLAGS) -DCOMMIT=$(shell git rev-list --count --all "
      "2>/dev/null || echo 0) $^ -o $@\n\n"
      "endif\n\n"
      "install:\n"
      "\t@echo \"NOT IMPL\"\n"
      "\texit 1\n\n"
      "uninstall:\n"
      "\t@echo \"NOT IMPL\"\n"
      "\texit 1\n\n"
      "clean:\n"
      "\trm -rf c-out\n\n"
      "dist-clean: clean\n"
      "\trm -f config.mak\n\n"
      ".PHONY: all clean dist-clean install uninstall build format\n",
      mkfile_name, mkfile_name, mkfile_name, mkfile_name, fmt.project,
      mkfile_name, mkfile_name, mkfile_name);
  free (mkfile_name);
  if (fmt.clang_format)
    create_file_with_content (".clang-format",
                              "Language: Cpp\nBasedOnStyle: GNU\n");
  switch (fmt.licence)
    {
    case BSD3:
      create_file_with_content (
          "COPYING",
          "BSD 3-Clause License\n\nCopyright (c) %d, "
          "%s\n\nRedistribution and use in source and binary forms, "
          "with or without\nmodification, are permitted provided that the "
          "following conditions are met:\n\n1. Redistributions of source code "
          "must retain the above copyright notice, this\n   list of "
          "conditions and the following disclaimer.\n\n2. Redistributions in "
          "binary form must reproduce the above copyright notice,\n   this\n"
          "list of conditions and the following disclaimer in the "
          "documentation\n   and/or other materials provided with the "
          "distribution.\n\n3. Neither the name of the copyright holder nor "
          "the names of its\n   contributors may be used to endorse or "
          "promote products derived from\n   this software without specific "
          "prior written permission.\n\nTHIS SOFTWARE IS PROVIDED BY THE "
          "COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\nAND ANY EXPRESS OR "
          "IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\nIMPLIED "
          "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE "
          "ARE\nDISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR "
          "CONTRIBUTORS BE LIABLE\nFOR ANY DIRECT, INDIRECT, INCIDENTAL, "
          "SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\nDAMAGES (INCLUDING, BUT NOT "
          "LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\nSERVICES; LOSS OF "
          "USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\nCAUSED "
          "AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT "
          "LIABILITY,\nOR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN "
          "ANY WAY OUT OF THE USE\nOF THIS SOFTWARE, EVEN IF ADVISED OF THE "
          "POSSIBILITY OF SUCH DAMAGE.\n",
          YEAR, fmt.name);
      break;
    case GPLv3:
    default:
      break;
    }
  create_and_enter_directory (fmt.project);
  create_file_with_content (
      "main.c",
      "#include <stdio.h>\n\nint main(void) {\n  printf(\"%s: Hello "
      "%s!\\n\");\nreturn 0;\n}",
      fmt.project ? fmt.project : DEFAULT_PROJECT_NAME,
      fmt.name ? fmt.name : "World");
  return 0;
}
