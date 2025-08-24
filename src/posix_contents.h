/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

// clang-format off
#ifndef POSIX_CONTENTS_H
#define POSIX_CONTENTS_H

#define line(ln) ln "\n"

char *readme =
  line ("%s ( concise description )")
  line ()
  line ("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor")
  line ("incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis")
  line ("nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.")
  line ("Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu")
  line ("fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in")
  line ("culpa qui officia deserunt mollit anim id est laborum.");

char *configure =
  line ("#!/bin/sh")
  line ()
  line ("usage() {")
  line ("cat <<EOF")
  line ("Usage: $0 [OPTION]... [VAR=VALUE]...")
  line ()
  line ("To assign environment variables (e.g., CC, CFLAGS...), specify them as")
  line ("VAR=VALUE.")
  line ()
  line ("  CC      C compiler command [detected]")
  line ("  CFLAGS  C compiler flags [-g, ...]")
  line ()
  line ("EOF")
  line ("exit 0")
  line ("}")
  line ()
  line ("echo () { printf \"%%s\\n\" \"$*\" ; }")
  line ("cmdexists () { type \"$1\" >/dev/null 2>&1 ; }")
  line ("trycc () { test -z \"$CC\" && cmdexists \"$1\" && CC=$1 ; }")
  line ()
  line ("prefix=/usr/bin/")
  line ("CFLAGS=\"-Wall -Wextra -O2\"")
  line ("LDFLAGS=")
  line ("CC=")
  line ()
  line ("for arg ; do")
  line ("case \"$arg\" in")
  line ("--help|h) usage ;;")
  line ("CFLAGS=*) CFLAGS=${arg#*=} ;;")
  line ("LDFLAGS=*) LDFLAGS=${arg#*=} ;;")
  line ("esac")
  line ("done")
  line ()
  line ("printf \"checking for C compiler... \"")
  line ("%%s")
  line ("printf \"%%s\\n\" \"$CC\"")
  line ()
  line ("printf \"checking weather C compiler works... \"")
  line ("status=\"fail\"")
  line ("tmpc=\"$(mktemp -d)/test.c\"")
  line ("echo \"typedef int x;\" > \"$tmpc\"")
  line ("if output=$($CC $CFLAGS -c -o /dev/null \"$tmpc\" 2>&1) ; then")
  line ("printf \"yes\\n\"")
  line ("else")
  line ("printf \"no; %%s\\n\" \"$output\"")
  line ("exit 1")
  line ("fi")
  line ()
  line ("printf \"creating config.mak... \"")
  line ("printf \"PREFIX=%%s\\n\" \"$prefix\" > config.mak")
  line ("printf \"CFLAGS=%%s\\n\" \"$CFLAGS\" >> config.mak")
  line ("printf \"LDFLAGS=%%s\\n\" \"$LDFLAGS\" >> config.mak")
  line ("printf \"CC=%%s\\n\" \"$CC\" >> config.mak")
  line ("printf \"done\\n\"");

char *makefile =
  line ("prefix = /usr/bin")
  line ()
  line ("%s_SRCS := $(wildcard %s/*.c)")
  line ("%s_OBJS := $(patsubst ./%s/%%.c,c-out/obj/%%.o,$(%s_SRCS))")
  line ()
  line ("%s := c-out/bin/%s")
  line ()
  line ("-include config.mak")
  line ()
  line ("ifeq ($(wildcard config.mak),)")
  line ("all:")
  line ("\t@echo \"File config.mak not found, run configure\"")
  line ("\t@exit 1")
  line ("else")
  line ()
  line ("all: build $(%s)")
  line ()
  line ("build:")
  line ("\tmkdir -p c-out/bin")
  line ("\tmkdir -p c-out/obj")
  line ()
  line ("c-out/obj/%%.o: %s/%%.c")
  line ("\t$(CC) $(CFLAGS) -c $< -o $@")
  line ()
  line ("$(%s): $(%s_OBJS)")
  line ("\t$(CC) $(CFLAGS) -DCOMMIT=$(shell git rev-list --count --all 2>/dev/null || echo 0) $^ -o $@")
  line ()
  line ("endif")
  line ()
  line ("install:")
  line ("\t@echo \"NOT IMPL\"")
  line ("\texit 1")
  line ()
  line ("uninstall:")
  line ("\t@echo \"NOT IMPL\"")
  line ("\texit 1")
  line ()
  line ("clean:")
  line ("\trm -rf c-out")
  line ()
  line ("dist-clean: clean")
  line ("\trm -f config.mak")
  line ()
  line (".PHONY: all clean dist-clean install uninstall");

char *clang_format =
  line ("BasedOnStyle: GNU");

char *config_h =
  line ("#ifndef CONFIG_H")
  line ("#define CONFIG_H")
  line ()
  line ("/* Program information */")
  line ("#define PROGRAM \"%s\"")
  line ("#define LICENSE_LINE  \"%s\"")
  line ("#define AUTHORS \"%s\"")
  line ("#define VERSION \"pre-alpha\"")
  line ("#define YEAR 2025")
  line ()
  line ("#define HELP_REQUESTED 2")
  line ("#define ERROR_MEMORY_ALLOCATION 3")
  line ("#define ERROR_DIRECTORY_CREATION 4")
  line ()
  line ("#endif");

char *main_c =
  line ("#include <stdio.h>")
  line ()
  line ("int main(void) {")
  line ("  printf(\"%s: Hello %s!\\n\");")
  line ("  return 0;")
  line ("}");

char *main_c_gnu =
  line ("#include <stdio.h>")
  line ("#include \"standard.h\"")
  line ()
  line ("void usage(int status) {")
  line ("  fprintf(stderr, \"Usage: %s [OPTION...]\\n\");")
  line ("  fprintf(stderr, \"    --help\\tdisplay the help text and exit\\n\");")
  line ("  fprintf(stderr, \"    --version\\toutput version information and exit\\n\");")
  line ("}")
  line ()
  line ("int main(int argc, char **argv) {")
  line ("  parse_standard_options(usage, argc, argv);")
  line ("  printf(\"%s: Hello %s!\\n\");")
  line ("  return 0;")
  line ("}");

char *standard_c =
  line ("#include \"standard.h\"")
  line ("#include \"../config.h\"")
  line ("#include <stdio.h>")
  line ("#include <stdlib.h>")
  line ("#include <string.h>")
  line ()
  line ("int")
  line ("parse_standard_options (void (*usage) (int), int argc, char **argv)")
  line ("{")
  line ("  for (int i = 1; i < argc; ++i)")
  line ("    {")
  line ("      if (strcmp (argv[i], \"--help\") == 0)")
  line ("        {")
  line ("          usage (0);")
  line ("          exit (EXIT_SUCCESS);")
  line ("        }")
  line ("      else if (strcmp (argv[i], \"--version\") == 0)")
  line ("        {")
  line ("          printf (\"%s %s %d\\nCopyright (C) %d %s.\\n%s\\nThis is free software: you are free to change and redistribute it.\\nThere is NO WARRNTY, to the extent permitted by law.\\n\", PROGRAM, VERSION, COMMIT, YEAR, AUTHORS, LICENSE_line );")
  line ("          exit (EXIT_SUCCESS);")
  line ("        }")
  line ("    }")
  line ("  return HELP_REQUESTED;")
  line ("}");

char *standard_h =
  line ("#ifndef STANDARD_H")
  line ("#define STANDARD_H")
  line ()
  line ("int parse_standard_options(void (*usage_func)(), int argc, char **argv);")
  line ()
  line ("#endif");

char *what_next =
  line ("# What next?")
  line ("")
  line ("## Steps")
  line (" - Write a description in the README")
  line (" - Write the usage function ( write to stderr )")
  line (" - Start writing your program");

#endif
// clang-format on
