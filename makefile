override MAKEFLAGS += -rR
override OUTPUT := ba
override CC := cc -m32
override LD := ld -m elf_i386
override CC_FLAGS := -Os -ffreestanding -nostdlib \
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
		    -z max-page-size=0x1000 -z common-page-size=0x10
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

# Default target
.PHONY: all
all: bin/$(OUTPUT)

# Target-specific variables for Windows
.PHONY: windows
windows: OUTPUT := ba-windows
windows: CC_FLAGS += -DTARGET_WINDOWS
windows: bin/ba-windows

# Target-specific variables for DekOS
.PHONY: dekos
dekos: OUTPUT := ba-dekos
dekos: CC_FLAGS += -DTARGET_DEKOS
dekos: bin/ba-dekos

# Pattern rule for both targets
bin/ba-windows bin/ba-dekos bin/ba: $(OBJ) | bin
	$(LD) -T ba.ld $(OBJ) -o $@.tmp $(LDFLAGS)
	# Merge text and data into one segment
	objcopy --merge-notes --remove-section=.comment \
	        --remove-section=.note --remove-section=.note.gnu.build-id \
	        $@.tmp $@
	$(STRIP) $@
	chmod +x $@
	rm -f $@.tmp
	@SIZE=$$(wc -c < $@); \
	echo "BA compiler ($@): $$SIZE bytes"; \
	if [ $$SIZE -le 8192 ]; then \
		echo "✓ Under 8KB! (insane)"; \
	elif [ $$SIZE -le 10240 ]; then \
		echo "✓ Under 10KB! (amazing)"; \
	elif [ $$SIZE -le 12288 ]; then \
		echo "✓ Under 12KB! (great)"; \
	else \
		echo "✗ $$(($$SIZE - 12288)) bytes over 12KB"; \
	fi

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

# Size analysis
.PHONY: size
size:
	@echo "=== Section sizes ==="
	@objdump -h bin/$(OUTPUT)
	@echo "=== Total ==="
	@wc -c < bin/$(OUTPUT) | awk '{print $$1 " bytes"}'

# Build both targets
.PHONY: all-targets
all-targets: windows dekos