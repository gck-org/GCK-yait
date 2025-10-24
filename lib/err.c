/*
 *   gcklib.err - Contains color printing functions
 *
 *   CONFIGURATION
 *       #define NOCOLOR
 *           Force no color when printing
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"

#define RESET "\x1B[0m"
#define ERROR "\x1B[1;91m"
#define WARN "\x1B[1;95m"
#define NOTE "\x1B[1;94m"
#define HINT "\x1B[38;5;166m"

static bool err_support_color(void)
{
#ifdef NOCOLOR
	return false;
#else
	static int cached = -1;
	if (cached != -1)
		return cached;
	const char *term, *colorterm, *force, *nocolor;
	term = getenv("TERM");
	colorterm = getenv("COLORTERM");
	force = getenv("FORCE_COLOR");
	nocolor = getenv("NO_COLOR");
	if (nocolor && *nocolor && (!force || !*force)) {
		cached = 0;
		return false;
	}
	if (force && *force && strcmp(force, "0") != 0) {
		cached = 1;
		return true;
	}
	// if (!isatty(fileno(stdout))) {
	// 	cached = 0;
	// 	return false;
	// }
	if (colorterm && *colorterm) {
		cached = 1;
		return true;
	}
	if (!term || !*term) {
		cached = 0;
		return false;
	}
	if (strstr(term, "color") || strstr(term, "xterm") ||
	    strstr(term, "screen") || strstr(term, "vt100") ||
	    strstr(term, "rxvt") || strstr(term, "ansi") ||
	    strstr(term, "linux") || strstr(term, "konsole") ||
	    strstr(term, "vte") || strstr(term, "kitty") ||
	    strstr(term, "wezterm") || strstr(term, "gnome")) {
		cached = 1;
		return true;
	}
	cached = 0;
	return false;
#endif
}

void errorf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (err_support_color()) {
		fprintf(stderr, "%serror%s: ", ERROR, RESET);
	} else {
		fputs("error: ", stderr);
	}

	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	va_end(args);
}
void fatalf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (err_support_color()) {
		fprintf(stderr, "%sfatal error%s: ", ERROR, RESET);
	} else {
		fputs("fatal error: ", stderr);
	}

	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	va_end(args);

	fputs("program terminated.\n", stderr);
	exit(EXIT_FAILURE);
}
void warnf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (err_support_color()) {
		fprintf(stderr, "%swarning%s: ", WARN, RESET);
	} else {
		fputs("warning: ", stderr);
	}

	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	va_end(args);
}
void notef(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (err_support_color()) {
		fprintf(stderr, "%snote%s: ", NOTE, RESET);
	} else {
		fputs("note: ", stderr);
	}

	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	va_end(args);
}
void hintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (err_support_color()) {
		fprintf(stderr, "%shint: ", HINT);
	} else {
		fputs("hint: ", stderr);
	}

	vfprintf(stderr, format, args);

	if (err_support_color()) {
		fprintf(stderr, "%s\n", RESET);
	} else {
		fputc('\n', stderr);
	}

	va_end(args);
}

void errorfa(int code)
{
	errorf(strerror(code));
}

void fatalfa(int code)
{
	fatalf(strerror(code));
}

void notefa(int code)
{
	notef(strerror(code));
}

void warnfa(int code)
{
	warnf(strerror(code));
}

void hintfa(int code)
{
	hintf(strerror(code));
}

/* end of file err.c */
