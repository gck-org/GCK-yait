/* Copyright (C) GCK
 *
 * This file is part of gcklib
 *
 * This project and file is licensed under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#ifndef fs_H
#define fs_H

#include <stdio.h>
#include <sys/types.h>

char *fs_read(const char *path);

bool fs_exists(const char *path);

int fs_append(const char *path, const char *format, ...);
int fs_del(const char *path);
int fs_new(const char *path);
int fs_write(const char *path, const char *format, ...);

FILE *fs_temp();

#endif
