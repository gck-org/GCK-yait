/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

int mkdir_p(const char *path)
{
	char *copypath = strdup(path);
	char *p = copypath;
	int status = 0;

	if (!copypath)
		return -1;

	if (copypath[0] == '/')
		p++;

	for (; *p; p++) {
		if (*p == '/') {
			*p = '\0';
			if (mkdir(copypath, 0777) && errno != EEXIST) {
				status = -1;
				break;
			}
			*p = '/';
		}
	}

	if (!status && mkdir(copypath, 0777) && errno != EEXIST)
		status = -1;

	free(copypath);
	return status;
}

int cfprintf(const char *path, const char *format, ...)
{
	char *dirpath;
	const char *slash = strrchr(path, '/');
	if (slash) {
		size_t len = slash - path;
		dirpath = malloc(len + 1);
		if (!dirpath)
			return -1;
		memcpy(dirpath, path, len);
		dirpath[len] = '\0';
		if (mkdir_p(dirpath)) {
			free(dirpath);
			return -1;
		}
		free(dirpath);
	}

	FILE *fp = fopen(path, "w");
	if (!fp)
		return -1;

	va_list args;
	va_start(args, format);
	if (vfprintf(fp, format, args) < 0) {
		va_end(args);
		fclose(fp);
		return -1;
	}
	va_end(args);

	fclose(fp);

	if (flast)
		fprintf(stderr, "Created files %d, done.\n", fno);
	else
		fprintf(stderr, "Created files %d\r", fno);
	fno++;

	return 0;
}
