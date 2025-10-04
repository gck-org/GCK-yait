/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flag.h"

static char *nextchar;

int getopt_long(int argc, char *const argv[], const char *optstring,
		const struct option *longopts, int *longindex)
{
	if (nextchar == NULL || *nextchar == '\0') {
		if (optind >= argc)
			return -1;
		if (argv[optind][0] != '-' || argv[optind][1] == '\0')
			return -1;
		if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
			optind++;
			return -1;
		}
		if (argv[optind][1] == '-') {
			const char *arg = argv[optind] + 2;
			char *eq = strchr(arg, '=');
			size_t len = eq ? (size_t)(eq - arg) : strlen(arg);
			for (int i = 0; longopts[i].name; i++) {
				if (strncmp(arg, longopts[i].name, len) == 0 &&
				    strlen(longopts[i].name) == len) {
					if (longindex)
						*longindex = i;
					if (longopts[i].has_arg ==
					    required_argument) {
						if (eq)
							optarg = (char *)eq + 1;
						else if (optind + 1 < argc)
							optarg = argv[++optind];
						else
							return '?';
					} else if (longopts[i].has_arg ==
						   optional_argument)
						optarg = eq ? (char *)eq + 1 :
							      NULL;
					else
						optarg = NULL;
					optind++;
					if (longopts[i].flag) {
						*longopts[i].flag =
							longopts[i].val;
						return 0;
					}
					return longopts[i].val;
				}
			}
			optind++;
			return '?';
		}
		nextchar = argv[optind] + 1;
	}
	char c = *nextchar++;
	const char *pos = strchr(optstring, c);
	if (!pos) {
		optopt = c;
		if (*nextchar == '\0')
			optind++;
		return '?';
	}
	if (pos[1] == ':') {
		if (*nextchar != '\0') {
			optarg = nextchar;
			optind++;
			nextchar = NULL;
		} else if (optind + 1 < argc) {
			optarg = argv[++optind];
			optind++;
			nextchar = NULL;
		} else {
			optopt = c;
			return '?';
		}
	} else {
		optarg = NULL;
		if (*nextchar == '\0') {
			optind++;
			nextchar = NULL;
		}
	}
	return c;
}
