#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define on_error(msg, code)                                                   \
  if (err)                                                                    \
    {                                                                         \
      fprintf (stderr, "\n");                                                 \
      printfn (msg ": %s", strerror (err));                                   \
      return code;                                                            \
    }
#else
#define on_error(msg, code)                                                   \
  if (code)                                                                   \
    {                                                                         \
      printfn (msg ": %s", strerror (code));                                  \
      return code;                                                            \
    }
#endif

#ifdef DEBUG
#define debug(fmt, ...)                                                       \
  fprintf (stderr, "\e[0;32m[%8d]\e[0;33m " fmt "\e[0m\n", __LINE__,          \
           ##__VA_ARGS__)
#define debugc(fmt, ...)                                                      \
  fprintf (stderr, "\e[0;32m[%8d]\e[0;33m " fmt "... \e[0m", __LINE__,        \
           ##__VA_ARGS__)
#define done fprintf (stderr, "done.\n")
#else
#define debug(fmt, ...) ((void)0)
#define debugc(fmt, ...) ((void)0)
#define done ((void)0)
#endif

#endif // DEBUG_H
