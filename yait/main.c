#include "../lib/print.h"
#include "core.h"

void create(format_t);

int main(int argc, char **argv) {
  if (argc < 2) {
    printfn("error: not enough arguments.");
    return 1;
  }
  format_t conf;
  conf.name = argv[1];
  conf.nogit = false;
  return 0;
}

void create(format_t conf) {
}
