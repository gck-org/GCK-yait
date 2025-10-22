/*
 *   gcklib.flag - Contains command-line flag parsing functions
 *
 *
 *   LICENSE: BSD-3-Clause
 *
 *   Copyright (c) 2025 GCK
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/* end of file flag.c */
