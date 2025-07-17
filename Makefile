prefix = /usr/bin

YAIT_SRCS := $(wildcard yait/*.c) $(wildcard core/*.c)
YAIT_OBJS := $(patsubst yait/%.c,c-out/obj/%.o,$(YAIT_SRCS))

YAIT := c-out/bin/yait

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: build $(YAIT) $(YAIT_DOC)

build:
	mkdir -p c-out/bin
	mkdir -p c-out/obj

c-out/obj/%.o: yait/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(YAIT): $(YAIT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@


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

.PHONY: all clean dist-clean install uninstall build format
