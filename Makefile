prefix=/usr/bin

YAIT_SRCS=yait/main.c
YAIR_DOC_SRCS=yait-doc/main.c

YAIT_OBJS = $(addprefix obj/yait,$(patsubst yait/%,obj/%,$(patsubst %.c,%.o,$(notdir $(YAIT_SRCS)))))
YAIT_DOC_OBJS = $(addprefix obj/yait-doc,$(patsubst yait-doc/%,obj/%,$(patsubst %.c,%.o,$(notdir $(YAIT_DOC_SRCS)))))

YAIT=$(prefix)/yait
YAIT_DOC=$(prefix)/yait-doc

-include config.mak

ifeq ($(wildcard config.mak),)
all:
	@echo "File config.mak not found, run configure"
	@exit 1
else

all: clean obj bin $(YAIT) $(YAIT_DOC)

obj:
	mkdir -p $@/yait
	mkdir -p $@/yait-doc

bin:
	mkdir -p $@

obj/yait/%.o: yait/**/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/yait-doc/%.o: yait-doc/**/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(YAIT): 
	$(CC) $(YAIT_SCRS) -o ./bin/yait

$(YAIT_DOC): 
	$(CC) $(YAIT_DOC_SRCS) -o ./bin/yait-doc

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
	rm config.mak

.PHONY: all clean dist-clean install uninstall
