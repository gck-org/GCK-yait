/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <stdio.h>
#include <stdlib.h>

#include "err.h"

#include "xmem.h"

void *ensure_nonnull(void *ptr)
{
	if (ptr == NULL)
		fatalf("memory exhausted");
	return ptr;
}

void *xmalloc(size_t size)
{
	return ensure_nonnull(malloc(size));
}

void *xrealloc(void *ptr, size_t size)
{
	return ensure_nonnull(realloc(ptr, size));
}

void *xcalloc(size_t nmemb, size_t size)
{
	return ensure_nonnull(calloc(nmemb, size));
}
