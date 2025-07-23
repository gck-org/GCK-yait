// Usage: yait [OPTION]... [PROJECT] (NAME)

#include "../config.h"
#include "../core/file.h"
#include "../core/print.h"
#include "../core/standard.h"
#include "contents.h"
#include "format.h"
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG

#define DEFAULT_USER_NAME "unknown"
#define DEFAULT_PROJECT_NAME "Project"
#define DEFAULT_LICENSE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true

#define print_option(option, description)                                     \
  printf ("        %-20s %-20s\n", option, description)

#ifdef DEBUG
#define on_error(msg, code)                                                   \
  if (err)                                                                    \
    {                                                                         \
      fprintf (stderr, "\n");                                                 \
      printfn (msg ": %s", strerror (err));                                   \
      return code;                                                            \
    }
#else
#define on_error(msg, code)                                                   \
  if (err)                                                                    \
    {                                                                         \
      printfn (msg ": %s", strerror (err));                                   \
      return code;                                                            \
    }
#endif

#ifdef DEBUG
#define debug(fmt, ...)                                                       \
  fprintf (stderr, "\e[0;32m[%8d]\e[0;33m " fmt "\e[0m\n", __LINE__,          \
           ##__VA_ARGS__)
#define debugc(fmt, ...)                                                      \
  fprintf (stderr, "\e[0;32m[%8d]\e[0;33m " fmt "... \e[0m", __LINE__,        \
           ##__VA_ARGS__)
#define done fprintf (stderr, "done.\n")
#else
#define debug(fmt, ...)
#define debugc(fmt, ...)
#define done
#endif

// int create_license_if_needed (format_t);
// int get_license_line_and_create_license (format_t, char **);
int create_configure ();
int create_makefile (format_t);
int create_project (format_t);
int generate_source_code (format_t);
int maybe_apply_clang_format (format_t);
int reset_path_ ();
int sanitize (format_t *);
int setup_git (format_t);

int depth;

void
usage (int status)
{
  if (status != 0)
    {
      fprintf (stderr, "Try 'yait --help' for more information.\n");
      return;
    }

  printf ("Usage: yait [OPTION]... [PROJECT] (NAME)\n");
  printf ("Creates a C project with opinionated defaults.\n");
  printf ("When only given the first argument it will detect your name.\n\n");
  printf ("Mandatory arguments to long options are mandatory for short "
          "options too\n");
  print_option ("-l, --license=NAME",
                "Set license (gpl, mit, bsd) [default: bsd]");
  print_option ("--use-cpp", "Uses the CPP language instead of C");
  print_option ("--git", "Initialize git repository");
  print_option ("--GNU", "Adds standard GNU argument parsing to your project");
  printf ("    --help     display this help text and exit\n");
  printf ("    --version  output version information and exit\n");
}

int
main (int argc, char **argv)
{
  int err;

  if (argc < 2)
    {
      printfn ("error: not enough arguments.");
      return 1;
    }

  err = initialize_main (&argc, &argv);
  err = parse_standard_options (usage, argc, argv);

  if (err && err != HELP_REQUESTED)
    {
      printfn ("error: %s", strerror (err));
      return err;
    }

  format_t conf = { 0 };

  conf.project = argv[0];
  conf.name = (argc > 1) ? argv[1] : NULL;

  //TODO: Iterate over arguments and if it dosn't start with a '-' treat it as argv[0] and argv[1]

  if (!conf.name)
    {
      struct passwd *pw = getpwuid (getuid ());
      conf.name = (pw && pw->pw_name) ? pw->pw_name : DEFAULT_USER_NAME;
    }

  conf.flag.git = DEFAULT_GIT_INIT;
  conf.flag.clang_format = DEFAULT_CLANG_FORMAT;
  conf.licence = DEFAULT_LICENSE;

  err = create_project (conf);
  if (!err)
    debug ("project made successfully");
  else
    debug ("something when wrong");

  return err;
}

int
create_project (format_t fmt)
{
  int err;

  debugc ("sanitize");
  err = sanitize (&fmt);
  on_error ("failed to sanitize format", err);
  done;

  debugc ("take %s", fmt.project);
  err = create_and_enter_directory (fmt.project);
  depth = 0;
  on_error ("failed to create or enter directory", err);
  done;

  // debug ("create licenseing");
  // err = create_license_if_needed (fmt);
  // on_error ("failed to create license", err);

  debugc ("create makefile");
  err = create_makefile (fmt);
  on_error ("failed to create Makefile", err);
  done;

  debug ("setup git");
  err = setup_git (fmt);
  if (err)
    printfn ("warning: git initialization failed: %s", strerror (err));

  debug ("create .clang-format");
  err = maybe_apply_clang_format (fmt);
  if (err)
    printfn ("warning: clang-format setup failed: %s", strerror (err));

  debugc ("generate source code");
  err = generate_source_code (fmt);
  on_error ("failed to generate source code", err);
  done;

  return 0;
}

#define reset_path reset_path_ ()
int
reset_path_ ()
{
  while (depth != 0)
    {
      if (chdir ("..") != 0)
        return errno;
      else
        depth--;
    }
  return 0;
}

int
sanitize (format_t *fmt)
{
  if (!fmt->project)
    fmt->project = DEFAULT_PROJECT_NAME;
  if (!fmt->name)
    fmt->name = DEFAULT_USER_NAME;
  if (fmt->licence != BSD3 && fmt->licence != GPLv3 && fmt->licence != MIT)
    fmt->licence = DEFAULT_LICENSE;
  fmt->flag.git = fmt->flag.git ? true : DEFAULT_GIT_INIT;
  fmt->flag.clang_format
      = fmt->flag.clang_format ? true : DEFAULT_CLANG_FORMAT;
  fmt->flag.GNU = fmt->flag.GNU ? true : false;

  return 0;
}

// int
// create_license_if_needed (format_t fmt)
// {
//   char *license_line = NULL;
//   return get_license_line_and_create_license (fmt, &license_line);
// }
//
// int
// get_license_line_and_create_license (format_t fmt, char
// **license_line_buffer)
// {
//   switch (fmt.licence)
//     {
//     case BSD3:
//       *license_line_buffer = "License BSD-3-Clause: BSD-3-Clause "
//                              "<https://opensource.org/license/bsd-3-clause/>";
//       return create_file_with_content ("COPYING", bsd3_license_template,
//       YEAR,
//                                        fmt.name);
//
//     case GPLv3:
//     default:
//       *license_line_buffer = "License GPLv3: GNU GPL version 3 "
//                              "<https://www.gnu.org/licenses/gpl-3.0.html>";
//       return create_file_with_content ("COPYING", gplv3_license_template,
//       YEAR,
//                                        fmt.name);
//     }
// }

int
maybe_apply_clang_format (format_t fmt)
{
  if (!fmt.flag.clang_format)
    return 0;

  reset_path;

  char *clang_fmt = "BasedOnStyle: LLVM\nIndentWidth: 2\nUseTab: Never\n";
  return create_file_with_content (".clang-format", clang_fmt, 0, NULL);
}

int
setup_git (format_t fmt)
{
  if (!fmt.flag.git)
    return 0;

  reset_path;

  int err = system ("git init --quiet");
  if (err)
    printfn ("failed on git initialize: %s", strerror (err));

  return err;
}

int
create_makefile (format_t fmt)
{
  goto debug_skip;
  char *makefile_name = strdup (fmt.project);
  if (!makefile_name)
    {
      printfn ("fatal: out of memory");
      return 1;
    }

  for (char *p = makefile_name; *p; ++p)
    if (*p >= 'a' && *p <= 'z')
      *p -= 32;

  reset_path;

  create_file_with_content ("Makefile", makefile_template, makefile_name,
                            makefile_name, makefile_name, makefile_name,
                            makefile_name, makefile_name, fmt.project,
                            makefile_name);

  free (makefile_name);
debug_skip:
  return 0;
}

int
create_configure ()
{
  reset_path;

  create_file_with_content ("configure", configure_template);
  int err = system ("chmod +x configure");
  if (err)
    printfn ("error: %s", strerror (err));
  return err;
}

int
generate_source_code (format_t fmt)
{
  int err;

  debug ("take %s/%s", fmt.project, fmt.project);
  err = create_and_enter_directory (fmt.project);
  on_error ("failed to create or enter directory", err);

  if (fmt.flag.GNU)
    {
      debug ("GNU flag source branch");

      create_file_with_content ("main.c", main_c_gnu_template, fmt.project, fmt.name);

      goto atexit_clean;
    }

  debug ("default sourcebranch");
  create_file_with_content ("main.c", main_c_template, fmt.project, fmt.name);

atexit_clean:
  reset_path;
  return 0;
}
