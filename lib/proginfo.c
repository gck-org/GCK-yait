/*
 *   gcklib.proginfo - Program information functions, macros, and variables
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <config.h>

#include "proginfo.h"

/* TODO(vx-clutch): default this to argv[0] */
const char *prog_name = "";

void set_prog_name(char *name)
{
	prog_name = prog_name ? basename(name) : "";
}

void emit_try_help()
{
	printf("Try '%s --help' for more information\n", prog_name);
}

void emit_version()
{
	printf("\
%s %s %d\n\
Copyright (C) %d GCK.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRNTY, to the extent permitted by law.\n\
",
	       prog_name, VERSION, COMMIT, YEAR);
}

int parse_standard_options(int argc, char **argv, void (*print_help)(),
			   void (*print_version)())
{
	for (int i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "--help")) {
			print_help();
			exit(EXIT_SUCCESS);
		} else if (!strcmp(argv[i], "--version")) {
			emit_version();
			exit(EXIT_SUCCESS);
		}
	}
	return 0;
}

/* end of file proginfo.c */
