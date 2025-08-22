#ifndef UTIL_H
#define UTIL_H

#include "../include/yait.h"

licence_t TOlicence(const char *s);

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

int getopt_long(int argc, char *const argv[], const char *optstring,
		const struct option *longopts, int *longindex);

#endif
