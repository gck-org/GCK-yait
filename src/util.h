#ifndef UTIL_H
#define UTIL_H

#include "../include/yait.h"

licence_t TOlicence(const char *s);

int getopt_long(int argc, char *const argv[], const char *optstring,
		const struct option *longopts, int *longindex);

int mkdir_p(const char *path);
int cfprintf(const char *path, const char *format, ...);

#endif
