prefix = /usr/bin

YAIT_SRCS := $(wildcard src/*.c)
YAIT_OBJS := $(patsubst src/%.c,build/obj/%.o,$(YAIT_SRCS))

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
	mkdir -p build/obj

build/obj/%.o: src/%.c config.mak
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(YAIT): $(YAIT_OBJS) 
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
	$(RM) -rf build

dist-clean: clean
	$(RM) -f config.mak

.PHONY: all clean dist-clean install uninstall build
