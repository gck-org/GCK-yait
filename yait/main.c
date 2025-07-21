#include "../config.h"
#include "../core/file.h"
#include "../core/print.h"
#include "../core/standard.h"
#include "format.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "contents.h"

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
  conf.flag.git = DEFAULT_GIT_INIT;
  conf.flag.clang_format = DEFAULT_CLANG_FORMAT;

  conf.flag.GNU = true; // debug

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
  if (fmt.flag.git)
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
      readme_template,
      fmt.project ? fmt.project : DEFAULT_PROJECT_NAME);
  create_file_with_content (
      "configure",
      configure_template);
  int status = system("chmod +x configure");
  if (status)
    {
      printfn ("error: %s", strerror (status));
      return status;
    }
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
      makefile_template,
      mkfile_name, mkfile_name, mkfile_name, mkfile_name, mkfile_name,
      fmt.project, mkfile_name, mkfile_name, mkfile_name);
  free (mkfile_name);
  if (fmt.flag.clang_format)
    create_file_with_content (".clang-format", clang_format_template);
  char *license_line = "";
  switch (fmt.licence)
    {
    case BSD3:
      license_line = "License BSD-3-Clause: BSD-3-Clause <https://opensource.org/licence/bsd-3-clause>";
      create_file_with_content (
          "COPYING",
          bsd3_license_template,
          YEAR, fmt.name);
      break;
    case GPLv3:
    default:
      break;
    }
  create_file_with_content ("config.h",
                            config_h_template,
                            fmt.project, license_line, fmt.name, YEAR);
  create_and_enter_directory (fmt.project);
  if (!fmt.flag.GNU)
    {
      create_file_with_content (
          "main.c",
          main_c_template,
          fmt.project ? fmt.project : DEFAULT_PROJECT_NAME,
          fmt.name ? fmt.name : "World");
    }
  else
    {
      create_file_with_content (
          "main.c",
          main_c_gnu_template,
          fmt.project, fmt.project ? fmt.project : DEFAULT_PROJECT_NAME,
          fmt.name ? fmt.name : "World");
    }
  if (fmt.flag.GNU)
    {
      create_file_with_content (
          "standard.c",
          standard_c_template);
      create_file_with_content (
          "standard.h",
          standard_h_template);
    }
  return 0;
}
