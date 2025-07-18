#ifndef FILE_H
#define FILE_H

#include <unistd.h>

int take(const char *dirname);

int touch(char *, char *, ...);
int dir(char *, ...);

#endif
