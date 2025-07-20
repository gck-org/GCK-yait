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

EMBED_FILES := $(wildcard embed/*)
gen-file-embeds:
	@echo "// Generated header with embedded files" > $(EMBED_HEADER)
	@echo "#ifndef EMBEDDED_FILES_H" >> $(EMBED_HEADER)
	@echo "#define EMBEDDED_FILES_H" >> $(EMBED_HEADER)
	@echo "" >> $(EMBED_HEADER)
	@for f in $(EMBED_FILES); do \
		var=$$(echo $$f | tr './-' '__' | tr '[:lower:]' '[:upper:]'); \
		xxd -i $$f | sed "s/^unsigned char/${var}_data/" | sed "s/^unsigned int/${var}_len/" >> $(EMBED_HEADER); \
		echo "" >> $(EMBED_HEADER); \
	done
	@echo "#endif // EMBEDDED_FILES_H" >> $(EMBED_HEADER)

c-out/obj/%.o: yait/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(YAIT): $(YAIT_OBJS)
	$(CC) $(CFLAGS) -DCOMMIT=$(shell git rev-list --count --all) $^ -o $@


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

.PHONY: all clean dist-clean install uninstall build format gen-file-embeds
