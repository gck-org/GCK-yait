#include "../config.h"
#include "../core/file.h"
#include "../core/print.h"
#include "../core/standard.h"
#include "format.h"
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create (format_t);

void usage () {};

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      printfn ("error: not enough arguments.");
      return 1;
    }
  int status = parse_standard_options (usage, argc, argv);
  if (status && status != 1)
    {
      printfn ("error: %s", strerror (status));
      return 1;
    }
  format_t conf;
  conf.project = argv[1];
  if (argc > 2)
    conf.name = argv[2];
  else
    {
      struct passwd *pw = getpwuid (getuid ());
      conf.name = pw ? pw->pw_name : NULL;
    }
  conf.git = true;
  conf.clang_format = true;
  conf.licence = BSD3;
  create (conf);
  return 0;
}

int
create (format_t fmt)
{
  error_t err = take (fmt.project);
  if (!err.null)
    {
      printfn ("failed to create or enter directory: %s", err.src);
      return 1;
    }
  if (fmt.git)
    system ("git init --quiet");
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
         fmt.project);
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
  if (fmt.clang_format)
    touch (".clang-format", "Language: Cpp\nBasedOnStyle: GNU\n");
  switch (fmt.licence)
    {
    case BSD3:
      touch (
          "COPYING",
          "BSD 3-Clause License\n\nCopyright (c) %d, "
          "%s\n\nRedistribution and use in source and binary forms, "
          "with or without\nmodification, are permitted provided that the "
          "following conditions are met:\n\n1. Redistributions of source code "
          "must retain the above copyright notice, this\n   list of "
          "conditions and the following disclaimer.\n\n2. Redistributions in "
          "binary form must reproduce the above copyright notice,\n   this "
          "list of conditions and the following disclaimer in the "
          "documentation\n   and/or other materials provided with the "
          "distribution.\n\n3. Neither the name of the copyright holder nor "
          "the names of its\n   contributors may be used to endorse or "
          "promote products derived from\n   this software without specific "
          "prior written permission.\n\nTHIS SOFTWARE IS PROVIDED BY THE "
          "COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\nAND ANY EXPRESS OR "
          "IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\nIMPLIED "
          "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE "
          "ARE\nDISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR "
          "CONTRIBUTORS BE LIABLE\nFOR ANY DIRECT, INDIRECT, INCIDENTAL, "
          "SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\nDAMAGES (INCLUDING, BUT NOT "
          "LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\nSERVICES; LOSS OF "
          "USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\nCAUSED "
          "AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT "
          "LIABILITY,\nOR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN "
          "ANY WAY OUT OF THE USE\nOF THIS SOFTWARE, EVEN IF ADVISED OF THE "
          "POSSIBILITY OF SUCH DAMAGE.\n",
          YEAR, fmt.name);
      break;
    default:
      break;
    }
  return 0;
}
