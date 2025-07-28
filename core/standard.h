/* Copyright (C) vx_clutch
 * 
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licence.
 * <https://opensource.org/license/bsd-3-clause>
 */

#ifndef STANDARD_H
#define STANDARD_H

/**
 * Parse standard command line options (--help, --version)
 * @param usage_func Function pointer to usage display function
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 if help/version requested, errno on error
 */
int parse_standard_options (void (*usage_func) (), int argc, char **argv);

/**
 * Setup the program
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 if something fails
 */
int initialize_main (int *, char ***);

#endif
