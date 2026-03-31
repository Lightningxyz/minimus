CC=i686-elf-gcc
LD=i686-elf-ld

CFLAGS=-ffreestanding -O2 -Wall -Wextra

all: os.img

boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

kernel.bin: kernel.c linker.ld
	$(CC) $(CFLAGS) -m32 -c kernel.c -o kernel.o
	$(LD) -m elf_i386 -T linker.ld -o kernel.bin kernel.o

os.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > os.img

run: os.img
	qemu-system-i386 -drive format=raw,file=os.img

clean:
	rm -f *.o *.bin *.img