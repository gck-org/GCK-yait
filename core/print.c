/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#include "yait.h"
#include <stdarg.h>
#include <stdio.h>

int printfn(char *format, ...)
{
	int len;
	va_list args;
	va_start(args, format);
	fprintf(stderr, "yait: ");
	len = vfprintf(stderr, format, args);
	fprintf(stderr, "\n"); /* Use stderr consistently */
	va_end(args);
	return len;
}
