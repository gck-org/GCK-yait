prefix = /usr/bin

YAIT_SRCS := $(wildcard src/*.c)

YAIT := bin/yait

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: build $(YAIT) $(YAIT_DOC)

build:
	mkdir bin

$(YAIT): $(YAIT_SRCS)
	$(CC) $(CFLAGS) -Iinclude -DCOMMIT=$(shell git rev-list --count --all) $^ -o $@

endif

install:
	@echo "NOT IMPL"
	exit 1

uninstall:
	@echo "NOT IMPL"
	exit 1

clean:
	$(RM) -rf bin

dist-clean: clean
	$(RM) -f config.mak

.PHONY: all clean dist-clean install uninstall build
