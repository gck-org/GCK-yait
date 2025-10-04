/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "err.h"

#include "fs.h"

char *fs_read(const char *path)
{
	FILE *fptr = fopen(path, "r");
	if (!fptr)
		return NULL;

	size_t cap = 1024;
	size_t len = 0;
	char *buf = malloc(cap);
	if (!buf) {
		fclose(fptr);
		return NULL;
	}

	int c;
	while ((c = fgetc(fptr)) != EOF) {
		if (len + 1 >= cap) {
			cap *= 2;
			char *tmp = realloc(buf, cap);
			if (!tmp) {
				free(buf);
				fclose(fptr);
				return NULL;
			}
			buf = tmp;
		}
		buf[len++] = (char)c;
	}
	buf[len] = '\0';

	fclose(fptr);
	return buf;
}

bool fs_exists(const char *path)
{
	FILE *fptr;
	bool exists;

	fptr = fopen(path, "r");
	if (fptr) {
		exists = true;
	} else {
		exists = false;
	}

	return exists;
}

int fs_append(const char *path, const char *format, ...)
{
	FILE *fp = fopen(path, "a");
	if (!fp)
		return -1;

	va_list ap;
	va_start(ap, format);
	int ret = vfprintf(fp, format, ap);
	va_end(ap);

	if (ret < 0) {
		fclose(fp);
		return -1;
	}

	if (fclose(fp) != 0)
		return -1;

	return ret;
}

int fs_del(const char *path)
{
	return remove(path);
}

int fs_new(const char *path)
{
	size_t len;
	int fd;

	if (path == NULL) {
		errno = EINVAL;
		return -1;
	}

	len = strlen(path);
	if (len == 0) {
		errno = EINVAL;
		return -1;
	}

	if (path[len - 1] == '/') {
		if (mkdir(path, 0777) == -1)
			return -1;
	} else {
		fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0666);
		if (fd == -1)
			return -1;
		close(fd);
	}

	return 0;
}

int fs_write(const char *path, const char *format, ...)
{
	FILE *fptr = fopen(path, "w");
	if (!fptr)
		return -1;

	va_list ap;
	va_start(ap, format);
	int ret = vfprintf(fptr, format, ap);
	va_end(ap);

	if (ret < 0) {
		fclose(fptr);
		return -1;
	}

	if (fclose(fptr) != 0)
		return -1;

	return ret;
}

FILE *fs_temp()
{
	FILE *fptr = tmpfile();

	if (!fptr) {
		errorf("tmpfile failed");
		return NULL;
	}

	return fptr;
}
