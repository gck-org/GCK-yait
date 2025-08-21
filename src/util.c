#include <string.h>

#include "util.h"
#include "../include/yait.h"

// clang-format off
licence_t TOlicence(const char *s)
{
	if (strcmp(s, "mit")) return MIT;
	if (strcmp(s, "gpl")) return GPL;
	if (strcmp(s, "bsd")) return BSD;
	return UNL;
}
// clang-format on
