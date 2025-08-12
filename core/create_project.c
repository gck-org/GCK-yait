/* Copyright (C) vx_clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>

#include "manifest.h"
#include "contents.h"
#include "yait.h"

#define DEFAULT_USER_NAME "unknown"
#define DEFAULT_PROJECT_NAME "Project"
#define DEFAULT_LICENCE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true
#define DEFAULT_GNU false

int program_exists(const char *prog)
{
	char *path = getenv("PATH");
	if (!path)
		return -1;

	char *copy = strdup(path);
	if (!copy)
		return -1;

	char *dir = strtok(copy, ":");
	while (dir) {
		char buf[4096];
		snprintf(buf, sizeof(buf), "%s/%s", dir, prog);
		if (access(buf, X_OK) == 0) {
			free(copy);
			return 0;
		}
		dir = strtok(NULL, ":");
	}

	free(copy);
	return 1;
}

int sanitize(manifest_t *m)
{
	if (!m->project)
		m->project = DEFAULT_PROJECT_NAME;

	if (!m->name) {
		struct passwd *pw = getpwuid(getuid());
		m->name = (pw && pw->pw_name) ? pw->pw_name : DEFAULT_USER_NAME;
	}

	if (m->licence == UNLICENCE)
		m->licence = DEFAULT_LICENCE;

	if (!m->flag.git)
		m->flag.git = DEFAULT_GIT_INIT;

	if (!m->flag.clang_format)
		m->flag.clang_format = DEFAULT_CLANG_FORMAT;

	if (!m->flag.GNU)
		m->flag.GNU = DEFAULT_GNU;

	return 0;
}

int create_libraries(manifest_t manifest)
{
	int status = 0;

	if (!manifest.libraries) {
		return status;
	}

	if (HAS_LIBRARY(manifest.libraries, LIB_RAYLIB)) {
		REMOVE_LIBRARY(manifest.libraries, LIB_RAYLIB);
		status = system(
			"git submodule add -q https://github.com/raysan5/raylib");
		if (status != 0)
			return status;
	}

	if (HAS_LIBRARY(manifest.libraries, LIB_NCURSES)) {
		REMOVE_LIBRARY(manifest.libraries, LIB_NCURSES);
		status = system(
			"git submodule add -q https://github.com/mirror/ncurses");
		if (status != 0)
			return status;
	}

	if (HAS_LIBRARY(manifest.libraries, LIB_CURL)) {
		REMOVE_LIBRARY(manifest.libraries, LIB_CURL);
		status = system(
			"git submodule add -q https://github.com/curl/curl");
		if (status != 0)
			return status;
	}

	return status;
}

int create_licence(manifest_t manifest, char **licence_line_buffer)
{
	if (manifest.licence == UNLICENCE)
		return 0;

	if (!licence_line_buffer)
		return EINVAL;

	switch (manifest.licence) {
	case BSD3:
		*licence_line_buffer = "Bsd";
		fprintf(stderr,
			"create_licence: BSD3 license generation not implemented\n");
		return ENOSYS;
	case GPLv3:
		fprintf(stderr,
			"create_licence: GPLv3 license generation not implemented\n");
		return ENOSYS;
	case MIT:
		fprintf(stderr,
			"create_licence: MIT license generation not implemented\n");
		return ENOSYS;
	default:
		fprintf(stderr, "create_licence: unknown license type\n");
		return EINVAL;
	}
}

int maybe_create_clang_format(manifest_t manifest)
{
	if (!manifest.flag.clang_format)
		return 0;

	int status = create_file_with_content(".clang-format",
					      clang_format_template);
	return status;
}

int setup_git(manifest_t manifest)
{
	if (!manifest.flag.git)
		return 0;

	int status = system("git init --quiet");
	if (status == -1) {
		fprintf(stderr, "...: %s\n", strerror(errno));
	} else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		fprintf(stderr, "...: exited with status %d\n",
			WEXITSTATUS(status));
	}

	return status;
}

int create_makefile(manifest_t manifest)
{
	char *m = strdup(manifest.project);
	char *M = strdup(manifest.project);
	if (!M) {
		fprintf(stderr, "create_makefile: fatal: out of memory\n");
		return ENOMEM;
	}

	for (char *p = M; *p; ++p)
		if (*p >= 'a' && *p <= 'z')
			*p -= 'a' - 'A';

	int status = create_file_with_content(strcat(m, "Makefile"),
					      makefile_template, M, m, M, m, M,
					      M, m, M, m, M, m);

	free(m);
	free(M);
	return status;
}

int create_configure(manifest_t manifest)
{
	const char *cc;
	if (manifest.flag.use_cpp) {
		cc = "trycc g++\ntrycc CC\ntrycc clang++\n";
	} else {
		cc = "trycc gcc\ntrycc cc\ntrycc clang\n";
	}

	int status =
		create_file_with_content("configure", configure_template, cc);
	if (status != 0)
		return status;

	status = system("chmod +x configure");
	if (status != 0)
		fprintf(stderr, "create_configure: chmod failed: %s\n",
			strerror(status));

	return status;
}

int generate_source_code(manifest_t manifest, char *licence_line)
{
	int status = 0;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	int year = tm.tm_year + 1900;

	status = create_file_with_content("config.h", config_h_template,
					  manifest.project, licence_line, year);
	if (status != 0) {
		fprintf(stderr,
			"generate_source_code: failed to create config.h: %s\n",
			strerror(status));
		return status;
	}

	if (status != 0) {
		fprintf(stderr,
			"generate_source_code: failed to create or enter directory: %s\n",
			strerror(status));
		return status;
	}

	if (manifest.flag.GNU) {
		status = create_file_with_content(
			strcat(manifest.project, "main.c"), main_c_gnu_template,
			manifest.project, manifest.name);
	} else {
		status = create_file_with_content(
			strcat(manifest.project, "main.c"), main_c_template,
			manifest.project, manifest.name);
	}

	return status;
}

int create_project(manifest_t manifest)
{
	int status;

	sanitize(&manifest);

	if (strcmp(manifest.path, ".") != 0) {
		status = mkdir(manifest.path, 0755);
		if (status != 0 && errno != EEXIST) {
			fprintf(stderr,
				"create_project: failed to create directory '%s': %s\n",
				manifest.path, strerror(errno));
			return errno;
		}

		status = chdir(manifest.path);
		if (status != 0) {
			fprintf(stderr,
				"create_project: failed to enter directory '%s': %s\n",
				manifest.path, strerror(errno));
			return errno;
		}
	} else {
		manifest.path = "";
	}

	// status = create_makefile(manifest);
	// if (status != 0) {
	// 	fprintf(stderr,
	// 		"create_project: failed to create Makefile: %s\n",
	// 		strerror(status));
	// 	return status;
	// }
	//
	// status = create_configure(manifest);
	// if (status != 0) {
	// 	fprintf(stderr,
	// 		"create_project: failed to create configure: %s\n",
	// 		strerror(status));
	// 	return status;
	// }
	//
	// status = maybe_create_clang_format(manifest);
	// if (status != 0) {
	// 	fprintf(stderr,
	// 		"create_project: warning: clang-format setup failed: %s\n",
	// 		strerror(status));
	// }
	//
	// char *licence_line = malloc(1024);
	// if (!licence_line) {
	// 	fprintf(stderr,
	// 		"create_project: failed to allocate memory for licence line\n");
	// 	return ENOMEM;
	// }
	//
	// status = create_licence(manifest, &licence_line);
	// if (status != 0) {
	// 	fprintf(stderr,
	// 		"create_project: failed to create licence: %s\n",
	// 		strerror(status));
	// 	free(licence_line);
	// 	return status;
	// }
	//
	// status = generate_source_code(manifest, licence_line);
	// if (status != 0) {
	// 	fprintf(stderr,
	// 		"create_project: failed to generate source code: %s\n",
	// 		strerror(status));
	// 	free(licence_line);
	// 	return status;
	// }
	//
	// free(licence_line);
	//
	// status = create_libraries(manifest);
	// if (status != 0) {
	// 	printfn("failed to get libraries: %s", strerror(status));
	// 	return status;
	// }
	//
	// status = setup_git(manifest);
	// if (status != 0) {
	// 	printfn("warning: git initialization failed: %s",
	// 		strerror(status));
	// }

	char *resolved;
	resolved = malloc(PATH_MAX);
	if (!resolved) {
		printfn("Failed to alloc");
		return 2;
	}
	if (!realpath(manifest.path, resolved)) {
		printfn("Failed to get realpath");
		free(resolved);
		return 2;
	}
	fprintf(stderr, "Created %s at\n %s\n", manifest.project, resolved);

	return 0;
}
