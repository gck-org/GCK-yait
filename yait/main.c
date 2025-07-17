#include "../core/print.h"
#include "../core/file.h"
#include "format.h"

int create(format_t);

int main(int argc, char **argv) {
  if (argc < 2) {
    printfn("error: not enough arguments.");
    return 1;
  }

  format_t conf;
  conf.name = argv[1];
  conf.nogit = false;

  create(conf);
  return 0;
}

int create(format_t fmt) {
  take(fmt.name);

  touch("README",
        "%s ( concise description )\n\n"
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis\n"
        "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
        "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu\n"
        "fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in\n"
        "culpa qui officia deserunt mollit anim id est laborum.",
        fmt.name);
  touch ("hello", "world");
  return 0;
}
