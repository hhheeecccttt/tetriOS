# ──────────────────────────────────────────────────────────
# Bare-metal x86 OS build system
# ──────────────────────────────────────────────────────────

NASM  := nasm
CC    := gcc
LD    := ld

CFLAGS  := -m32 -std=c11 -ffreestanding -fno-pic \
           -fno-stack-protector -mno-red-zone \
           -Wall -Wextra -O2

LDFLAGS := -m elf_i386 -T linker.ld --oformat binary

IMAGE   := os.img

# ──────────────────────────────────────────────────────────
.PHONY: all run run-gui clean

all: $(IMAGE)

# ──────────────────────────────────────────────────────────
# Bootloader (raw binary)
# ──────────────────────────────────────────────────────────
boot.bin: boot.asm
	$(NASM) -f bin -o $@ $<

# ──────────────────────────────────────────────────────────
# Kernel entry stub
# ──────────────────────────────────────────────────────────
entry.o: entry.asm
	$(NASM) -f elf32 -o $@ $<

# ──────────────────────────────────────────────────────────
# Kernel C code
# ──────────────────────────────────────────────────────────
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c -o $@ $<

# ──────────────────────────────────────────────────────────
# Font file (THIS WAS MISSING — FIXES YOUR ERROR)
# ──────────────────────────────────────────────────────────
font.o: font.c
	$(CC) $(CFLAGS) -c -o $@ $<

# ──────────────────────────────────────────────────────────
# Link kernel + font into flat binary
# ──────────────────────────────────────────────────────────
kernel.bin: entry.o kernel.o font.o
	$(LD) $(LDFLAGS) -o $@ $^

# ──────────────────────────────────────────────────────────
# Disk image
# ──────────────────────────────────────────────────────────
$(IMAGE): boot.bin kernel.bin
	cat boot.bin kernel.bin > $@
	truncate -s 512K $@
	@echo ""
	@echo "Image ready: $(IMAGE)"
	@echo "Run: make run"

# ──────────────────────────────────────────────────────────
# Run in QEMU (text mode)
# ──────────────────────────────────────────────────────────
run: $(IMAGE)
	qemu-system-i386 \
	    -drive format=raw,file=$(IMAGE) \
	    -nographic \
	    -serial mon:stdio

# ──────────────────────────────────────────────────────────
# Run in GUI mode (VGA window)
# ──────────────────────────────────────────────────────────
run-gui: $(IMAGE)
	qemu-system-i386 \
	    -drive format=raw,file=$(IMAGE)

# ──────────────────────────────────────────────────────────
# Clean build
# ──────────────────────────────────────────────────────────
clean:
	rm -f *.o *.bin $(IMAGE)