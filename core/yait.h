/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

/* Constants for file operations */
#define DEFAULT_DIR_PERMISSIONS 0755
#define MAX_PATH_LENGTH 1024

int printfn(char *format, ...);

int create_and_enter_directory(const char *dirname);

int create_file_with_content(char *path, char *format, ...);

int create_directory(char *format, ...);

int parse_standard_options(void (*usage_func)(), int argc, char **argv);

int program_exists(const char *prog);

#include "manifest.h"
int create_project(manifest_t manifest);

#endif // YAIT_H
