/* Copyright (C) GCK
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/licence/bsd-3-clause>
 */

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>

#include "../lib/str_dup.h"

#include "name.h"

char *get_name()
{
	int fds[2];
	if (pipe(fds) == -1)
		goto sysuser;

	pid_t pid = fork();
	if (pid == -1) {
		close(fds[0]);
		close(fds[1]);
		goto sysuser;
	}

	if (pid == 0) {
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
		execlp("git", "git", "config", "--get", "user.name",
		       (char *)NULL);
		_exit(127);
	}

	close(fds[1]);
	char buf[256];
	ssize_t n = read(fds[0], buf, sizeof buf - 1);
	close(fds[0]);
	int status;
	waitpid(pid, &status, 0);
	if (n > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		buf[n] = 0;
		buf[strcspn(buf, "\n")] = 0;
		return str_dup(buf);
	}

sysuser: {
	char *name = getlogin();
	if (name)
		return str_dup(name);
	struct passwd *pw = getpwuid(getuid());
	if (pw && pw->pw_name)
		return str_dup(pw->pw_name);
}
	return "author";
}
