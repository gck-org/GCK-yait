#include "../config.h"
#include "../core/file.h"
#include "../core/print.h"
#include "../core/standard.h"
#include "contents.h"
#include "format.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG

#define DEFAULT_USER_NAME      "unknown"
#define DEFAULT_PROJECT_NAME   "Project"
#define DEFAULT_LICENSE        BSD3
#define DEFAULT_GIT_INIT       true
#define DEFAULT_CLANG_FORMAT   true

#define print_option(option, description) \
  printf("        %-20s %-20s\n", option, description)


int create_license_if_needed(format_t);
int create_makefile(format_t);
int create_project(format_t);
int get_license_line_and_create_license(format_t, char**);
int maybe_apply_clang_format(format_t);
int sanitize(format_t *);
int setup_git_and_permissions(format_t);

void
usage(int status)
{
  if (status != 0)
    {
      fprintf(stderr, "Try 'yait --help' for more information.\n");
      return;
    }

  printf("Usage: yait [OPTION]... [PROJECT] (NAME)\n");
  printf("Creates a C project with opinionated defaults.\n");
  printf("When only given the first argument it will detect your name.\n\n");
  printf("Mandatory arguments to long options are mandatory for short options too\n");
  print_option("-l, --license=NAME", "Set license (gpl, mit, bsd) [default: gpl]");
  print_option("--use-cpp", "Uses the CPP language instead of C");
  print_option("--git", "Initialize git repository");
  print_option("--GNU", "Adds standard GNU argument parsing to your project");
  printf("                --help     display this help text and exit\n");
  printf("                --version  output version information and exit\n");
}

int
main(int argc, char **argv)
{
  if (argc < 2)
    {
      printfn("error: not enough arguments.");
      return 1;
    }

  int status = initialize_main(&argc, &argv);
  status = parse_standard_options(usage, argc, argv);

  if (status && status != HELP_REQUESTED)
    {
      printfn("error: %s", strerror(status));
      return status;
    }

  format_t conf;
  conf.project = argv[0];
  conf.name = (argc > 2) ? argv[1] : NULL;

  if (!conf.name)
    {
      struct passwd *pw = getpwuid(getuid());
      conf.name = (pw && pw->pw_name) ? pw->pw_name : DEFAULT_USER_NAME;
    }

  conf.flag.git = DEFAULT_GIT_INIT;
  conf.flag.clang_format = DEFAULT_CLANG_FORMAT;
  conf.licence = DEFAULT_LICENSE;
  #ifdef DEBUG
        system(strcat("rm -rf ", conf.project));
  #endif
  return create_project(conf);
}

int
create_project(format_t fmt)
{
  int err;

  err = create_and_enter_directory(fmt.project);
  if (err)
    {
      printfn("failed to create or enter directory: %s", strerror(err));
      return err;
    }

  err = sanitize(&fmt);
  if (err)
    {
      printfn("failed to sanitize format: %s", strerror(err));
      return err;
    }

  err = create_license_if_needed(fmt);
  if (err)
    {
      printfn("failed to create license: %s", strerror(err));
      return err;
    }

  err = create_makefile(fmt);
  if (err)
    {
      printfn("failed to create Makefile: %s", strerror(err));
      return err;
    }

  err = setup_git_and_permissions(fmt);
  if (err)
    {
      printfn("warning: git initialization failed: %s", strerror(err));
      // continue even if git fails
    }

  err = maybe_apply_clang_format(fmt);
  if (err)
    {
      printfn("warning: clang-format setup failed: %s", strerror(err));
    }

  return 0;
}

int
sanitize(format_t *fmt)
{
  if (!fmt->name)
    fmt->name = DEFAULT_USER_NAME;

  return 0;
}

int
create_license_if_needed(format_t fmt)
{
  char *license_line = NULL;
  return get_license_line_and_create_license(fmt, &license_line);
}

int
get_license_line_and_create_license(format_t fmt, char **license_line_buffer)
{
  switch (fmt.licence)
    {
    case BSD3:
      *license_line_buffer =
          "License BSD-3-Clause: BSD-3-Clause <https://opensource.org/licence/bsd-3-clause>";
      return create_file_with_content("COPYING", bsd3_license_template, YEAR, fmt.name);

    case GPLv3:
    default:
      *license_line_buffer = "License GPLv3: GNU GPL version 3 <https://www.gnu.org/licenses/gpl-3.0.html>";
      return create_file_with_content("COPYING", gplv3_license_template, YEAR, fmt.name);
    }
}

int
maybe_apply_clang_format(format_t fmt)
{
  if (!fmt.flag.clang_format)
    return 0;

  const char *clang_fmt = "BasedOnStyle: LLVM\nIndentWidth: 2\nUseTab: Never\n";
  return create_file_with_content(".clang-format", clang_fmt, 0, NULL);
}

int
setup_git_and_permissions(format_t fmt)
{
  if (!fmt.flag.git)
    return 0;

  int err = system("git init --quiet");
  if (err)
    {
      printfn("failed on git initialize: %s", strerror(err));
    }
  return err;
}
