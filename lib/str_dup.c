#include <stdlib.h>
#include <string.h>
#include "xmem.h"

#include "str_dup.h"

char *str_dup(char *s)
{
	char *new = xmalloc(strlen(s) + 1);
	strcpy(new, s);
	return new;
}
