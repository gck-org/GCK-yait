/* Copyright (C) GCK
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef YAIT_H
#define YAIT_H

typedef enum { MIT, GPL, BSD } licence_t;

typedef struct {
	licence_t licence;
	char *project;
	char *author;
	char *editor;
} manifest_t;

int create_project(manifest_t manifest);

#endif // YAIT_H
