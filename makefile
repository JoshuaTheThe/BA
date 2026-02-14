override MAKEFLAGS += -rR
override OUTPUT := ba
override CC := cc -m32
override LD := ld -m elf_i386
override CC_FLAGS := -std=c89 -Os -ffreestanding -nostdlib \
                     -fno-stack-protector -fno-builtin \
                     -fomit-frame-pointer -fno-unwind-tables \
                     -fno-asynchronous-unwind-tables -fno-ident \
                     -fdata-sections -ffunction-sections \
                     -fno-jump-tables -fno-tree-switch-conversion \
                     -fno-strict-aliasing -fno-common \
                     -falign-functions=1 -falign-jumps=1 -falign-loops=1 \
                     -mpreferred-stack-boundary=2 -minline-all-stringops \
                     -I src/include -fno-pic
override LDFLAGS := --build-id=none --gc-sections \
                    -z norelro --no-eh-frame-hdr \
                    --strip-all -s -O2 \
		    -z max-page-size=0x1000 -z common-page-size=0x1000
override STRIP := strip --strip-all \
                  --remove-section=.note* \
                  --remove-section=.comment \
                  --remove-section=.eh_frame \
                  --remove-section=.eh_frame_hdr \
                  --remove-section=.jcr \
                  --remove-section=.got \
                  --remove-section=.got.plt \
                  --remove-section=.plt \
                  --remove-section=.data \
                  --remove-section=.rodata

override C_SRC := src/ba.c
override AS_SRC := src/crt.S
override OBJ := $(addprefix obj/,$(notdir $(C_SRC:.c=.o))) \
                $(addprefix obj/,$(notdir $(AS_SRC:.S=.o)))

.PHONY: all
all: bin/$(OUTPUT)
	@SIZE=$$(wc -c < bin/$(OUTPUT)); \
	echo "BA compiler: $$SIZE bytes"; \
	if [ $$SIZE -le 8192 ]; then \
		echo "✓ Under 8KB! (insane)"; \
	elif [ $$SIZE -le 10240 ]; then \
		echo "✓ Under 10KB! (amazing)"; \
	elif [ $$SIZE -le 12288 ]; then \
		echo "✓ Under 12KB! (great)"; \
	else \
		echo "✗ $$(($$SIZE - 12288)) bytes over 12KB"; \
	fi

# Merge all sections into one to save space
bin/$(OUTPUT): $(OBJ) | bin
	$(LD) -T ba.ld $(OBJ) -o $@.tmp $(LDFLAGS)
	# Merge text and data into one segment
	objcopy --merge-notes --remove-section=.comment \
	        --remove-section=.note --remove-section=.note.gnu.build-id \
	        $@.tmp $@
	$(STRIP) $@
	chmod +x $@
	rm -f $@.tmp

# Compile with -ffreestanding to avoid builtins
obj/%.o: src/%.c | obj
	$(CC) $(CC_FLAGS) -ffreestanding -c $< -o $@

obj/%.o: src/%.S | obj
	$(CC) $(CC_FLAGS) -c $< -o $@

obj:
	mkdir -p obj

bin:
	mkdir -p bin

# Ultra-clean
.PHONY: clean
clean:
	rm -rf obj bin

.PHONY: distclean
distclean: clean
	rm -f *~

# Size analysis
.PHONY: size
size: bin/$(OUTPUT)
	@echo "=== Section sizes ==="
	@objdump -h bin/$(OUTPUT) | grep -E 'text|data|bss'
	@echo "=== Total ==="
	@wc -c < bin/$(OUTPUT) | awk '{print $$1 " bytes"}'