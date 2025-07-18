#include "standard.h"
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_standard_options(void (*usage)(), int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--help") == 0) {
      usage();
      exit(0);
    } else if (strcmp(argv[i], "--version") == 0) {
      printf("%s %s %d\n", PROGRAM, VERSION, COMMIT);
      exit(0);
    }
  }
  return -1;
}
