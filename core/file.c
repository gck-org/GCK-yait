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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "yait.h"

int create_file_with_content(char *path, char *format, ...)
{
	FILE *fp = fopen(path, "w");
	if (!fp) {
		return -1;
	}
	va_list args;
	va_start(args, format);
	vfprintf(fp, format, args);
	va_end(args);

	fclose(fp);
	return 0;
}

int create_directory(char *format, ...)
{
	va_list args;
	va_start(args, format);

	char path[MAX_PATH_LENGTH];
	int result = vsnprintf(path, sizeof(path), format, args);
	va_end(args);

	/* Check if the path was truncated */
	if (result >= (int)sizeof(path)) {
		return ENAMETOOLONG;
	}

	if (mkdir(path, DEFAULT_DIR_PERMISSIONS) < 0) {
		return errno;
	}

	return 0;
}

int create_and_enter_directory(const char *dirname)
{
	int err = create_directory("%s", dirname);
	if (err) {
		return err;
	}
	if (chdir(dirname) != 0) {
		return errno;
	}
	return 0;
}
