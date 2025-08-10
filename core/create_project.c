#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "manifest.h"
#include "contents.h"
#include "yait.h"

#define DEFAULT_USER_NAME "unknown"
#define DEFAULT_PROJECT_NAME "Project"
#define DEFAULT_LICENCE BSD3
#define DEFAULT_GIT_INIT true
#define DEFAULT_CLANG_FORMAT true

/* This is to keep track of how deep we are within
  the project tree. This is used in reset_path_ ()   */
int depth;

/* This macro exist purely because I like how it looks. This should be called
   in every function that creates file to ensure they are being created in
   right place.  */
#define reset_path reset_path_()
int reset_path_()
{
	while (depth != 0) {
		if (chdir("..") != 0)
			return errno;
		else
			--depth;
	}
	return 0;
}

int program_exists(const char *prog)
{
	char *path = getenv("PATH");
	if (!path)
		return 1;

	char *copy = strdup(path);
	if (!copy)
		return 1;

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

// TODO(vx-clutch): sanitize the alpha-numeric range
// clang-format off
 int sanitize(manifest_t *m)
{
	if (!m->project) m->project                = DEFAULT_PROJECT_NAME;
	if (!m->name) m->name 			   = DEFAULT_USER_NAME;
	if (!(m->licence == UNLICENCE)) m->licence = DEFAULT_LICENCE;

	m->flag.git = m->flag.git ? true : DEFAULT_GIT_INIT;
	m->flag.clang_format = m->flag.clang_format ? true : DEFAULT_CLANG_FORMAT;
	m->flag.GNU = m->flag.GNU ? true : DEFAULT_GNU;

	if (!m->name) {
		struct passwd *pw = getpwuid(getuid());
		m->name = (pw && pw->pw_name) ? pw->pw_name : DEFAULT_USER_NAME;
	}

	return 0;
}
// clang-format on

int create_libraries(manifest_t manifest)
{
	int status = 0;

	if (!manifest.libraries) {
		return status;
	}

	/* reset_path; */
	while (depth != 0) {
		if (chdir("..") != 0)
			return errno;
		else
			--depth;
	}

	for (int i = 0; i < LIB_COUNT_; ++i) {
		if (HAS_LIBRARY(manifest.libraries, LIB_RAYLIB)) {
			REMOVE_LIBRARY(manifest.libraries, LIB_RAYLIB);
			status = system(
				"git submodule add -q https://github.com/raysan5/raylib");
		} else if (HAS_LIBRARY(manifest.libraries, LIB_NCURSES)) {
			REMOVE_LIBRARY(manifest.libraries, LIB_NCURSES);
			status = system(
				"git submodule add -q https://github.com/mirror/ncurses");
		} else if (HAS_LIBRARY(manifest.libraries, LIB_CURL)) {
			REMOVE_LIBRARY(manifest.libraries, LIB_CURL);
			status = system(
				"git submodule add -q https://github.com/raysan5/raylib");
		}
	}
	return status;
}

int create_licence(manifest_t manifest, char **licence_line_buffer)
{
	if (manifest.licence == UNLICENCE)
		return 0;

	/* reset_path; */
	while (depth != 0) {
		if (chdir("..") != 0)
			return errno;
		else
			--depth;
	}

	assert(licence_line_buffer != NULL);

	switch (manifest.licence) {
	case BSD3:
		*licence_line_buffer = "Bsd";
		printfn("Not impl");
		assert(1 == 2);
		break;
	case GPLv3:
		printfn("Not impl");
		assert(1 == 2);
		break;
	case MIT:
		printfn("Not impl");
		assert(1 == 2);
		break;
	case UNLICENCE:
	default:
		printfn("bad logic in create_licence ()");
		return 1;
	}
	return 0;
}

int maybe_create_clang_format(manifest_t manifest)
{
	int status;
	if (!manifest.flag.clang_format)
		return 0;

	/* reset_path; */
	while (depth != 0) {
		if (chdir("..") != 0)
			return errno;
		else
			--depth;
	}

	status = create_file_with_content(".clang-format",
					  clang_format_template);

	return status;
}

int setup_git(manifest_t manifest)
{
	if (!manifest.flag.git) {
		return 0;
	}

	/* reset_path; */
	while (depth != 0) {
		if (chdir("..") != 0)
			return errno;
		else
			--depth;
	}

	int status = system("git init --quiet");
	if (status) {
		printfn("failed on git initialize: %s", strerror(status));
	}
	return status;
}

int create_makefile(manifest_t manifest)
{
	char *makefile_name = strdup(manifest.project);
	if (!makefile_name) {
		printfn("fatal: out of memory");
		return 1;
	}

	for (char *p = makefile_name; *p; ++p)
		if (*p >= 'a' && *p <= 'z')
			*p -= 32;

	reset_path;

	create_file_with_content("Makefile", makefile_template, makefile_name,
				 makefile_name, makefile_name, makefile_name,
				 makefile_name, makefile_name, manifest.project,
				 makefile_name, makefile_name);

	free(makefile_name);
	return 0;
}

int create_configure(manifest_t manifest)
{
	int status = 0;
	reset_path;

	char *cc;
	if (manifest.flag.use_cpp) {
		cc = "trycc g++\ntrycc CC\ntrycc clang++\n";
	} else {
		cc = "trycc gcc\ntrycc cc\ntrycc clang\n";
	}

	create_file_with_content("configure", configure_template, cc);
	status = system("chmod +x configure");
	if (status)
		printfn("error: %s", strerror(status));
	return status;
}

int generate_source_code(manifest_t manifest, char *licence_line)
{
	int status, year;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	year = tm.tm_year + 1900;

	// XXX(vx-clutch): this segfaults, but why?
	// status = create_file_with_content("config.h", manifest.project, licence_line, year);
	if (status) {
		printfn("failed to create config.h: %s", strerror(status));
		return status;
	}

	status = create_and_enter_directory(manifest.project);
	++depth;
	if (status) {
		printfn("failed to create or enter directory: %s",
			strerror(status));
		return status;
	}

	if (manifest.flag.GNU) {
		status = create_file_with_content("main.c", main_c_gnu_template,
						  manifest.project,
						  manifest.name);
	} else {
		status = create_file_with_content("main.c", main_c_template,
						  manifest.project,
						  manifest.name);
	}
	return status;
}

int create_project(manifest_t manifest)
{
	int status;

	status = sanitize(&manifest);
	if (status) {
		printfn("failed to sanitize format: %s", strerror(status));
		return status;
	}

	depth = 0;
	if (strcmp(manifest.path, ".") != 0) {
		status = create_and_enter_directory(manifest.project);
		if (status) {
			printfn("failed to create or enter directory: %s",
				strerror(status));
			return status;
		}
	}

	status = create_makefile(manifest);
	if (status) {
		printfn("failed to create Makefile: %s", strerror(status));
		return status;
	}

	status = create_configure(manifest);
	if (status) {
		printfn("failed to create configure: %s", strerror(status));
		return status;
	}

	status = setup_git(manifest);
	if (status) {
		printfn("warning: git initialization failed: %s",
			strerror(status));
	}

	status = maybe_create_clang_format(manifest);
	if (status) {
		printfn("warning: clang-format setup failed: %s",
			strerror(status));
	}

	// TODO(vx-clutch): make this smarter--or not ( macro ).
	char *licence_line = malloc(sizeof(char) * 1024);
	if (!licence_line) {
		printfn("failed to create memory for licence line: %s",
			strerror(status));
		return status;
	}
	status = create_licence(manifest, &licence_line);
	if (status) {
		printfn("failed to get libraries: %s", strerror(status));
		return status;
	}

	// TODO(vx-clutch): Take in licence line and put it into standard.c
	status = generate_source_code(manifest, licence_line);
	if (status) {
		printfn("failed to generate source code: %s", strerror(status));
		return status;
	}
	free(licence_line);

	status = create_libraries(manifest);
	if (status) {
		printfn("failed to get libraries: %s", strerror(status));
		return status;
	}

	return 0;
}
