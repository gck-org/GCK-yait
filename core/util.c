/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include "yait.h"

void emit_progress(const char *action, int count)
{
	if (count > 0) {
		fprintf(stderr, "%s %d", strcat("\r", action), count);
		fflush(stderr);
	} else {
		fprintf(stderr, ", done\n");
	}
}
