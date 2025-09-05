/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <ctype.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "../include/yait.h"

int fno = 1;
bool flast = false;

licence_t TOlicence(char *src)
{
	char *s = tostrupr(src);
	if (!strcmp(s, "MIT"))
		return MIT;
	if (!strcmp(s, "GPL"))
		return GPL;
	if (!strcmp(s, "BSD"))
		return BSD;
	free(s);
	return UNL;
}

char *str_dup(const char *s)
{
	char *new = malloc(strlen(s) + 1);
	if (!new)
		return NULL;
	strcpy(new, s);
	return new;
}

char *tostrupr(const char *s)
{
	char *new = malloc(strlen(s) + 1);
	if (!new)
		return NULL;
	strcpy(new, s);
	for (int i = 0; new[i] != '\0'; ++i)
		new[i] = toupper((unsigned char)new[i]);
	return new;
}

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
			const char *eq = strchr(arg, '=');
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
