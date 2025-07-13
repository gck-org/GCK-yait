prefix = /usr/bin

YAIT_SRCS := $(wildcard yait/*.c)
YAIT_DOC_SRCS := $(wildcard yait-doc/*.c)

YAIT_OBJS := $(patsubst yait/%.c,obj/yait/%.o,$(YAIT_SRCS))
YAIT_DOC_OBJS := $(patsubst yait-doc/%.c,obj/yait-doc/%.o,$(YAIT_DOC_SRCS))

YAIT := bin/yait
YAIT_DOC := bin/yait-doc

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: obj bin $(YAIT) $(YAIT_DOC)

obj:
	mkdir -p obj/yait obj/yait-doc

bin:
	mkdir -p bin

obj/yait/%.o: yait/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/yait-doc/%.o: yait-doc/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(YAIT): $(YAIT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(YAIT_DOC): $(YAIT_DOC_OBJS)
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
