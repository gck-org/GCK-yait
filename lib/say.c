/*
 *   gcklib.say - Provides printing and some string utilies
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

#include "say.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "xmem.h"

void alert()
{
	fputs("\a", stderr);
	fflush(stderr);
	return;
}

int vasprintf(char **result, const char *fmt, va_list ap)
{
	int total_width = strlen(fmt) + 1;
	*result = (char *)xmalloc(total_width);
	return vsprintf(*result, fmt, ap);
}

int asprintf(char **buf, const char *fmt, ...)
{
	int status;
	va_list ap;
	va_start(ap, fmt);
	status = vasprintf(buf, fmt, ap);
	va_end(ap);
	return status;
}

int say(const char *restrict format, ...)
{
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
		va_list args;
		va_start(args, format);
		int ret = vprintf(format, args);
		va_end(args);
		putchar('\n');
		fflush(stdout);
		return ret;
	}

	printf("\0337"); // save cursor (ESC 7)
	printf("\033[%d;1H", w.ws_row); // move to last row
	printf("\033[2K"); // clear entire line

	va_list args;
	va_start(args, format);
	int ret = vprintf(format, args); // print formatted message
	va_end(args);

	fflush(stdout); // ensure it's visible immediately
	printf("\0338"); // restore cursor (ESC 8)
	fflush(stdout);

	return ret;
}

_Noreturn void die(const char *msg)
{
	fputs(msg, stderr);
	exit(EXIT_FAILURE);
}

/* end of file say.c */
