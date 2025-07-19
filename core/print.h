#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h>
#include <stdio.h>

/**
 * Print a formatted message to stderr with program prefix and newline
 * @param format Format string (printf-style)
 * @param ... Variable arguments for formatting
 * @return Number of characters printed
 */
int print_error_with_prefix(char *format, ...);

/* Legacy function name for backward compatibility */
#define printfn print_error_with_prefix

#endif
