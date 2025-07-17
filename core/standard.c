#include "standard.h"
#include "../config.h"
#include "print.h"
#include <stdlib.h>
#include <string.h>

int
parse_standard_options (void (*usage) (), int argc, char **argv)
{
  for (int i = 1; i < argc; ++i)
    {
      if (strcmp (argv[i], "--help") == 0)
        {
          usage ();
          exit (0);
        }
      else if (strcmp (argv[i], "--version") == 0)
        {
          printfn ("%s (%s) %s\nCopyright (C) %d %s.\n%s\n"
                   "This is free software: you are free to change "
                   "and redistribute it.\nThere is NO WARRANTY, to the extent "
                   "permitted by law.",
                   PROGRAM, ORGANIZATION, VERSION, YEAR, ORGANIZATION,
                   LICENCE_LINE);
          exit (0);
        }
    }
  return -1;
}
