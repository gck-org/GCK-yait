/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#ifndef LICENCE_H
#define LICENCE_H

#include <stdlib.h>

#include "../include/yait.h"

static char *lget(licence_t licence)
{
	switch (licence) {
	case UNL:
		return "";
	case MIT:
		return "";
	case GPL:
		return "";
	case BSD:
		return "";

	case _LICENCE_COUNT_:
	default:
		abort();
	}
	return "";
}

#endif
