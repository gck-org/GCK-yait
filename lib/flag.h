/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#ifndef FLAG_H
#define FLAG_H

int getopt_long(int argc, char *const argv[], const char *optstring,
		const struct option *longopts, int *longindex);

#endif
