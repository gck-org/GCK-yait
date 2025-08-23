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
