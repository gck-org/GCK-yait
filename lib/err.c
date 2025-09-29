/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause license.
 * <https://opensource.org/licenses/BSD-3-Clause>
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"

#define RESET "\e[0m"
#define ERROR "\e[1;91m"
#define WARN "\e[1;95m"
#define NOTE "\e[1;94m"
#define HINT "\e[38;5;166m"

static bool __check(void)
{
	static int done = 0;
	static bool cached = false;

	if (!done) {
		const char *term = getenv("TERM");
		cached = isatty(STDOUT_FILENO) && term != NULL &&
			 (strstr(term, "color") != NULL ||
			  strstr(term, "ansi") != NULL ||
			  strstr(term, "xterm") != NULL);
		done = 1;
	}

	return cached;
}

void errorf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (__check()) {
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

	if (__check()) {
		fprintf(stderr, "%sfatal%s: ", ERROR, RESET);
	} else {
		fputs("fatal: ", stderr);
	}

	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	va_end(args);

	exit(EXIT_FAILURE);
}
void warnf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	if (__check()) {
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

	if (__check()) {
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

	if (__check()) {
		fprintf(stderr, "%shint: ", HINT);
	} else {
		fputs("hint: ", stderr);
	}

	vfprintf(stderr, format, args);

	if (__check()) {
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
