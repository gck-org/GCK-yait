PREFIX = /usr/bin/

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

install: $(YAIT)
	cp $(YAIT) $(PREFIX)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/yait

clean:
	$(RM) -r bin
	$(RM) -r build

dist-clean: clean
	$(RM) config.mak

.PHONY: all clean dist-clean install uninstall build
