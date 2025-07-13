prefix = /usr/bin

YAIT_SRCS := $(wildcard yait/*.c) $(wildcard lib/*.c)

YAIT_OBJS := $(patsubst yait/%.c,obj/yait/%.o,$(YAIT_SRCS))

YAIT := bin/yait

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: obj bin $(YAIT) $(YAIT_DOC)

obj:
	mkdir -p obj/yait

bin:
	mkdir -p bin

obj/yait/%.o: yait/%.c
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
	rm -rf obj bin

dist-clean: clean
	rm -f config.mak

.PHONY: all clean dist-clean install uninstall
