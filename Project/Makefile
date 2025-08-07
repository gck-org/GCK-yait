prefix = /usr/bin

PROJECT_SRCS := $(shell find . -name 'PROJECT/*.c')
PROJECT_OBJS := $(patsubst ./%.c,c-out/obj/%.o,$(PROJECT_SRCS))

PROJECT := c-out/bin/PROJECT

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: build $(Project)

build:
	mkdir -p c-out/bin
	mkdir -p c-out/obj

c-out/obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PROJECT): $(PROJECT_OBJS)
	$(CC) $(CFLAGS) -DCOMMIT=$(shell git rev-list --count --all 2>/dev/null || echo 0) $^ -o $@

endif

install:
	@echo "NOT IMPL"
	exit 1

uninstall:
	@echo "NOT IMPL"
	exit 1

clean:
	rm -rf c-out

dist-clean: clean
	rm -f config.mak

.PHONY: all clean dist-clean install uninstall
