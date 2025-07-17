#include "../core/file.h"
#include "../core/print.h"
#include "format.h"

int create (format_t);

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      printfn ("error: not enough arguments.");
      return 1;
    }
  format_t conf;
  conf.name = argv[1];
  conf.nogit = false;
  create (conf);
  return 0;
}

int
create (format_t fmt)
{
  error_t err = take(fmt.name);
  if (!err.null) {
    printfn("failed to create or enter directory: %s", err.src);
    return 1;
  }
  touch ("README",
         "%s ( concise description )\n\n"
         "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
         "eiusmod tempor\n"
         "incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
         "veniam, quis\n"
         "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
         "consequat.\n"
         "Duis aute irure dolor in reprehenderit in voluptate velit esse "
         "cillum dolore eu\n"
         "fugiat nulla pariatur. Excepteur sint occaecat cupidatat non "
         "proident, sunt in\n"
         "culpa qui officia deserunt mollit anim id est laborum.",
         fmt.name);
  touch ("configure",
         "#!/bin/sh\n"
         "\n"
         "usage() {\n"
         "cat <<EOF\n"
         "Usage: $0 [OPTION]... [VAR=VALUE]...\n"
         "\n"
         "To assign environment variables (e.g., CC, CFLAGS...), specify them "
         "as\n"
         "VAR=VALUE.\n"
         "\n"
         "  CC      C compiler command [detected]\n"
         "  CFLAGS  C compiler flags [-g, ...]\n"
         "\n"
         "EOF\n"
         "exit 0\n"
         "}\n"
         "\n"
         "echo () { printf \"%%s\\n\" \"$*\" ; }\n"
         "cmdexists () { type \"$1\" >/dev/null 2>&1 ; }\n"
         "trycc () { test -z \"$CC\" && cmdexists \"$1\" && CC=$1 ; }\n"
         "\n"
         "prefix=/usr/bin/\n"
         "CFLAGS=\"-Wall -Wextra -O2\"\n"
         "LDFLAGS=\n"
         "CC=\n"
         "\n"
         "for arg ; do\n"
         "case \"$arg\" in\n"
         "--help|h) usage ;;\n"
         "CFLAGS=*) CFLAGS=${arg#*=} ;;\n"
         "LDFLAGS=*) LDFLAGS=${arg#*=} ;;\n"
         "esac\n"
         "done\n"
         "\n"
         "printf \"checking for C compiler... \"\n"
         "trycc gcc\n"
         "trycc cc\n"
         "trycc clang\n"
         "printf \"%%s\\n\" \"$CC\"\n"
         "\n"
         "printf \"checking weather C compiler works... \"\n"
         "status=\"fail\"\n"
         "tmpc=\"$(mktemp -d)/test.c\"\n"
         "echo \"typedef int x;\" > \"$tmpc\"\n"
         "if output=$($CC $CFLAGS -c -o /dev/null \"$tmpc\" 2>&1) ; then\n"
         "printf \"yes\\n\"\n"
         "else\n"
         "printf \"no; %%s\\n\" \"$output\"\n"
         "exit 1\n"
         "fi\n"
         "\n"
         "printf \"creating config.mak... \"\n"
         "printf \"PREFIX=%%s\\n\" \"$prefix\" > config.mak\n"
         "printf \"CFLAGS=%%s\\n\" \"$CFLAGS\" >> config.mak\n"
         "printf \"LDFLAGS=%%s\\n\" \"$LDFLAGS\" >> config.mak\n"
         "printf \"CC=%%s\\n\" \"$CC\" >> config.mak\n"
         "printf \"done\\n\"\n");
  return 0;
}
