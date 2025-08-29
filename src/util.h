/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#ifndef UTIL_H
#define UTIL_H

#include "../include/yait.h"

licence_t TOlicence(char *s);

int getopt_long(int argc, char *const argv[], const char *optstring,
		const struct option *longopts, int *longindex);

char *str_dup(char *s);
char *tostrupr(char *s);

extern int fno;
extern bool flast;

int mkdir_p(const char *path);
int cfprintf(const char *path, const char *format, ...);

#endif
