/*
 *   gcklib.textc - Text control
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

#include <string.h>
#include <ctype.h>
#include "xmem.h"

#include "textc.h"

char *str_dup(char *s)
{
	char *new = xmalloc(strlen(s) + 1);
	strcpy(new, s);
	return new;
}

char *tostrupr(char *s)
{
	char *new = str_dup(s);
	for (int i = 0; new[i] != '\0'; ++i)
		new[i] = toupper((unsigned char)new[i]);
	return new;
}

char *tostrlwr(char *s)
{
	char *new = str_dup(s);
	for (int i = 0; new[i] != '\0'; ++i)
		new[i] = tolower((unsigned char)new[i]);
	return new;
}

char *textc_trim(char *s)
{
	return NULL;
}

char *textc_pad_left(int count, char *s, char pad)
{
	char *buffer = xmalloc(strlen(s) + 1);

	free(buffer);
	buffer = "NOT IMPLEMENTED";

	return buffer;
}

/* end of file textc.c */
