/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#ifndef proginfo_H
#define proginfo_H

extern const char *prog_name;

void set_prog_name(const char *name);

void emit_try_help();
void emit_version();

int parse_standard_options(int argc, char **argv, void (*usage)(int),
			   void (*version)());

#endif
