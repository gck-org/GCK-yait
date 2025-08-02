/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
   */

// Usage: yait [OPTION]... PROJECT [NAME]

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"
#include "../core/file.h"
#include "../core/print.h"
#include "../core/standard.h"
#include "contents.h"
#include "debug.h"
#include "format.h"

#define DEFAULT_USER_NAME "unknown"
#define DEFAULT_PROJECT_NAME "Project"
#define DEFAULT_LICENCE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true

/* This is to keep track of how deep we are within
  the project tree. This is used in reset_path_ ()   */
int depth;

#define print_option(option, description)                                     \
  printf ("        %-20s %-20s\n", option, description)

// clang-format off
static void
usage (int status)
{
  if (status != 0)
    {
      fprintf (stderr, "Try 'yait --help' for more information.\n");
      return;
    }

  printf ("Usage: yait [OPTION]... PROJECT [NAME]\n");
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

/* This macro exist purely because I like how it looks. This should be called
   in every function that creates file to ensure they are being created in
   right place.  */
#define reset_path reset_path_ ()
static int
reset_path_ ()
{
  while (depth != 0)
    {
      if (chdir ("..") != 0)
        return errno;
      else
        --depth;
    }
  return 0;
}

static int
sanitize (manifest_t *m)
{
  if (!m->project)
      m->project = DEFAULT_PROJECT_NAME;
  if (!m->name)
      m->name = DEFAULT_USER_NAME;
  if (!(m->licence == UNLICENCE))
      m->licence = DEFAULT_LICENCE;
  m->flag.git = m->flag.git ? true : DEFAULT_GIT_INIT;
  m->flag.clang_format = m->flag.clang_format ? true : DEFAULT_CLANG_FORMAT;
  m->flag.GNU = m->flag.GNU ? true : DEFAULT_GNU;

  return 0;
}

#define get(url) status = system ("git submodule add -q " url)
static int
create_libraries (manifest_t manifest)
{
  int status = 0;

  if (!manifest.libraries)
    {
      return status;
    }

  reset_path;

  for (int i = 0; i < LIB_COUNT_; ++i)
    {
      if HAS_LIBRARY (manifest.libraries, LIB_RAYLIB)
        {
          REMOVE_LIBRARY (manifest.libraries, LIB_RAYLIB);
          get ("https://github.com/raysan5/raylib");
        }
      else if HAS_LIBRARY (manifest.libraries, LIB_NCURSES)
        {
          REMOVE_LIBRARY (manifest.libraries, LIB_NCURSES);
          get ("https://github.com/mirror/ncurses");
        }
      else if HAS_LIBRARY (manifest.libraries, LIB_CURL)
        {
          REMOVE_LIBRARY (manifest.libraries, LIB_CURL);
          get ("https://github.com/raysan5/raylib");
        }
      reset_path;
    }

  return status;
}

static int
create_licence (manifest_t manifest, char **licence_line_buffer)
{
  if (manifest.licence == UNLICENCE)
    return 0;

  reset_path;
  /* TODO: Run better checks on licence_line_buffer to ensure we have enough
     space. This could be done through a multitude of ways; that is for you to
     figure out.  */
  assert (licence_line_buffer != NULL);

// TODO: Remove this and actually implement the features.
#define TODO()                                                                \
  printfn ("Not impl");                                                       \
  assert (1 == 2)

  switch (manifest.licence)
    {
    case BSD3:
      *licence_line_buffer = "Bsd";
      TODO ();
      break;
    case GPLv3:
      TODO ();
      break;
    case MIT:
      TODO ();
      break;
    case UNLICENCE:
    default:
      printfn ("bad logic in create_licence ()");
      return 1;
    }

  return 0;
}

static int
maybe_create_clang_format (manifest_t manifest)
{
  if (!manifest.flag.clang_format)
    return 0;

  reset_path;

  return create_file_with_content (".clang-format", clang_format_template);
}

static int
setup_git (manifest_t manifest)
{
  if (!manifest.flag.git)
    return 0;

  reset_path;

  int status = system ("git init --quiet");
  if (status)
    printfn ("failed on git initialize: %s", strerror (status));

  return status;
}

static int
create_makefile (manifest_t manifest)
{
  char *makefile_name = strdup (manifest.project);
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
                            makefile_name, makefile_name, manifest.project,
                            makefile_name, makefile_name);

  free (makefile_name);
  return 0;
}

static int
create_configure (manifest_t manifest)
{
  int status = 0;
  reset_path;

  char *cc;
  if (manifest.flag.use_cpp)
    {
      cc = "trycc g++\ntrycc CC\ntrycc clang++\n";
    }
  else
    {
      cc = "trycc gcc\ntrycc cc\ntrycc clang\n";
    }

  create_file_with_content ("configure", configure_template, cc);
  status = system ("chmod +x configure");
  if (status)
    printfn ("error: %s", strerror (status));
  return status;
}

static int
generate_source_code (manifest_t manifest)
{
  int status;

  debug ("take %s/%s", manifest.project, manifest.project);
  status = create_and_enter_directory (manifest.project);
  on_error ("failed to create or enter directory", status);
  ++depth;

  if (manifest.flag.GNU)
    {
      debug ("GNU flag source branch");

      create_file_with_content ("main.c", main_c_gnu_template,
                                manifest.project, manifest.name);

      goto atexit_clean;
    }

  debug ("default sourcebranch");
  create_file_with_content ("main.c", main_c_template, manifest.project,
                            manifest.name);

atexit_clean:
  reset_path;
  return 0;
}

static int
parse_arguments (manifest_t *conf, int argc, char **argv)
{
  static struct option long_options[] = {
    { "GNU", no_argument, 0, 1 },       { "use-cpp", no_argument, 0, 2 },
    { "git", no_argument, 0, 3 },       { "licence", required_argument, 0, 4 },
    { "lib", required_argument, 0, 5 }, { 0, 0, 0, 0 }
  };

  int opt;
  int long_index = 0;

  while ((opt = getopt_long (argc, argv, "", long_options, &long_index)) != -1)
    {
      switch (opt)
        {
        case 1:
          conf->flag.GNU = 1;
          break;
        case 2:
          conf->flag.use_cpp = 1;
          break;
        case 3:
          conf->flag.git = 1;
          break;
        case 4: // TODO: implement the licence options, and make it lowercase.
          break;
        case 5:
          ADD_LIBRARY (conf->libraries, TOlibrary (optarg)); // TODO: Get this working
          break;
        case '?':
          break;
        }
    }

  int positional_count = 0;
  for (int i = optind; i < argc; ++i)
    {
      if (argv[i][0] == '-')
        {
          fprintf (stderr, "Unknown flag: %s\n", argv[i]);
          continue;
        }

      if (positional_count == 0)
        conf->name = argv[i];
      else if (positional_count == 1)
        conf->project = argv[i];
      ++positional_count;
    }

  return 0;
}

static int
create_project (manifest_t manifest)
{
  int status;

  debugc ("sanitize");
  status = sanitize (&manifest);
  on_error ("failed to sanitize format", status);
  done;

  debugc ("take %s", manifest.project);
  status = create_and_enter_directory (manifest.project);
  depth = 0;
  on_error ("failed to create or enter directory", status);
  done;

  debugc ("create makefile");
  status = create_makefile (manifest);
  on_error ("failed to create Makefile", status);
  done;

  debug ("setup git");
  status = setup_git (manifest);
  if (status)
    {
      printfn ("warning: git initialization failed: %s", strerror (status));
    }

  debug ("create .clang-format");
  status = maybe_create_clang_format (manifest);
  if (status)
    {
      printfn ("warning: clang-format setup failed: %s", strerror (status));
    }

  debugc ("generate source code");
  status = generate_source_code (manifest);
  on_error ("failed to generate source code", status);
  done;

  debugc ("get libraries");
  status = create_libraries (manifest);
  on_error ("failed to get libraries", status);
  done;

  return 0;
}

int
main (int argc, char **argv)
{
  int status;

  if (argc < 2)
    {
      printfn ("error: not enough arguments.");
      return 1;
    }

  status = initialize_main (&argc, &argv);
  status = parse_standard_options (usage, argc, argv);

  if (status && status != HELP_REQUESTED)
    {
      printfn ("error: %s", strerror (status));
      return status;
    }

  manifest_t manifest = { 0 };

  parse_arguments (&manifest, argc, argv);

  if (!manifest.name) // TODO: Move to sanitize
    {
      struct passwd *pw = getpwuid (getuid ());
      manifest.name = (pw && pw->pw_name) ? pw->pw_name : DEFAULT_USER_NAME;
    }

  status = create_project (manifest);

  if (!status)
    debug ("project made successfully");
  else
    debug ("something when wrong");

  exit (status);
}
