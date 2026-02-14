override MAKEFLAGS += -rR
override OUTPUT := ba
override CC := cc
override AS := cc
override LD := cc
override CC_FLAGS := -std=c89 -Werror -c
override AS_FLAGS := -c
override STRIP := strip --strip-all --remove-section=.note* --remove-section=.comment

override C_SRC := $(shell cd src && find -L * -type f -name '*.c' | LC_ALL=c sort)
override AS_SRC := $(shell cd src && find -L * -type f -name '*.S' | LC_ALL=c sort)
override OBJ := $(addprefix obj/,$(C_SRC:.c=.c.o) $(AS_SRC:.S=.S.o))

.PHONY: all
all: bin/$(OUTPUT)

bin/$(OUTPUT): $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) -o $@
	$(STRIP) $@ -o $@

obj/%.c.o: src/%.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CC_FLAGS) -c $< -o $@

obj/%.S.o: src/%.S
	mkdir -p "$$(dirname $@)"
	$(AS) $(AS_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf obj bin
