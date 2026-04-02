# Cross-compiler toolchain
CC = i686-elf-gcc
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

# Compiler flags: freestanding (no libc), optimize, warn everything
CFLAGS = -ffreestanding -O2 -Wall -Wextra -Wno-unused-parameter -m32 -c

# Source files
C_SOURCES = kernel.c screen.c keyboard.c shell.c string.c
C_OBJECTS = $(C_SOURCES:.c=.o)

# Default target
all: os.img

# Assemble the bootloader (raw binary, 512 bytes)
boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

# Compile each C file to an object file
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Link all object files into an ELF, then extract flat binary
kernel.bin: $(C_OBJECTS)
	$(LD) -m elf_i386 -T linker.ld -o kernel.elf $(C_OBJECTS)
	$(OBJCOPY) -O binary kernel.elf kernel.bin

# Combine bootloader + kernel into a disk image
# Pad to at least 16 sectors (8192 bytes) so the bootloader reads enough
os.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > os.img
	dd if=/dev/zero bs=1 count=$$((8192 - $$(stat -f%z os.img))) >> os.img 2>/dev/null || true

# Run in QEMU
run: os.img
	qemu-system-i386 -rtc base=localtime -drive format=raw,file=os.img

# Clean build artifacts
clean:
	rm -f *.o *.bin *.elf *.img

.PHONY: all run clean