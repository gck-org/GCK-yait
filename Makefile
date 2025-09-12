PREFIX = /usr/bin/

YAIT_SRCS := $(wildcard src/*.c)
YAIT_OBJS := $(patsubst src/%.c,build/obj/%.o,$(YAIT_SRCS))

YAIT := bin/yait

ALLOWED_DIRS = doc include man src tools
DISTDIRS := $(sort $(shell find . -maxdepth 1 -type d -not -name '.' -printf '%f\n'))

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
	$(CC) $(CFLAGS) -DCOMMIT=$(shell git rev-list --count --all) -Iinclude -c $< -o $@

$(YAIT): $(YAIT_OBJS) 
	$(CC) $(CFLAGS) -Iinclude -DCOMMIT=$(shell git rev-list --count --all) $^ -o $@

endif

install: $(YAIT)
	cp $(YAIT) $(PREFIX)

uninstall:
	$(RM) $(PREFIX)yait

clean:
	$(RM) -r bin
	$(RM) -r build

distclean: clean
	$(RM) config.mak

check: $(YAIT)
	./src/tests/run_unit_tests.sh

syntax-check:
	@set -e; \
	fail=0; \
	for f in $(YAIT_SRCS); do \
		echo "$$f"; \
		if ! $(CC) $(CFLAGS) -fsyntax-only $$f; then \
			fail=1; \
		fi
	done; \
	if test $$fail -ne 0; then \
		exit 1;
	fi

distcheck: distclean
	@set -e; \
	for d in $(DESTDIRS); do \
		case " $(ALLOWED_DIRS) " in \
			*" $$d "*) ;; \
			*) echo "Error unexpected directory '$$d'"; exit 1 ;; \
		esac; \
	done

.PHONY: all clean dist-clean install uninstall build release check syntax-check distcheck
