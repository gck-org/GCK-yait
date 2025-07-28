/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef FILE_H
#define FILE_H

#include <unistd.h>

/* Constants for file operations */
#define DEFAULT_DIR_PERMISSIONS 0755
#define MAX_PATH_LENGTH 1024

/**
 * Create directory and change into it
 * @param dirname Directory name to create and enter
 * @return 0 on success, errno on failure
 */
int create_and_enter_directory (const char *dirname);

/**
 * Create a file with formatted content
 * @param path File path to create
 * @param format Format string for file content
 * @param ... Variable arguments for formatting
 * @return 0 on success, -1 on failure
 */
int create_file_with_content (char *path, char *format, ...);

/**
 * Create a directory with formatted path
 * @param format Format string for directory path
 * @param ... Variable arguments for formatting
 * @return 0 on success, errno on failure
 */
int create_directory (char *format, ...);

#endif
