prefix = /usr/bin

SAMPLE_PROJECT_SRCS := $(shell find . -name 'SAMPLE_PROJECT/*.c')
SAMPLE_PROJECT_OBJS := $(patsubst ./%.c,c-out/obj/%.o,$(SAMPLE_PROJECT_SRCS))

SAMPLE_PROJECT := c-out/bin/SAMPLE_PROJECT

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: build $(sample_project)

build:
	mkdir -p c-out/bin
	mkdir -p c-out/obj

c-out/obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SAMPLE_PROJECT): $(SAMPLE_PROJECT_OBJS)
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
