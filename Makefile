prefix = /usr/bin

YAIT_SRCS := $(wildcard src/*.c)

YAIT := bin/yait

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: build $(YAIT)

build:
	mkdir -p bin

$(YAIT): $(YAIT_SRCS) config.mak
	$(CC) $(CFLAGS) -Iinclude -DCOMMIT=$(shell git rev-list --count --all) $(YAIT_SRCS) -o $@

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
