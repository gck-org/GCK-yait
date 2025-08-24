/* Copyright (C) vx-clutch
 *
 * This file is part of yait
 *
 * This project and file is licenced under the BSD-3-Clause licecne
 * <https://opensource.org/licence/bsd-3-clause>
 */

// clang-format off
#ifndef LIBRARY_CONTENTS_H
#define LIBRARY_CONTENTS_H

#define line(ln) ln "\n"

char *makefile_lib =
	line ("CC = gcc")
	line ("CLFAGS = -Wall -Wextra -O2 -fPIC -Iinclude")
	line ("AR = ar rcs")
	line ("BUILD = build")
	line ("")
	line ("SRC = $(wildcard src/*.c)")
	line ("OBJ = $(SRC:src/%%.c=$(BUILD)/obj/%%.o)")
	line ("")
	line ("STATIC = $(BUILD)/%s.a")
	line ("SHARED = $(BUILD)/%s.so")
	line ("")
	line ("all: $(STAITC) $(SHARED)")
	line ("")
	line ("$(STATIC): $(OBJ)\n\t$(AR) $@ $^")
	line ("")
	line ("$(SHARED): $(OBJ)\n\t$(CC) -shared -o $@ $^")
	line ("")
	line ("$(BUILD)/%%.o: src/%%.c | $(BUILD)\n\t$(CC) $(CFLAGS) -c $< -o $@")
	line ("")
	line ("$(BUILD):\n\tmkdir -p $(BUILD)/obj")
	line ("")
	line ("clean:\n\t$(RM) -r $(BUILD)");

#endif
// clang-format on
