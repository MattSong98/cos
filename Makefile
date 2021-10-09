########################
# TOOLCHAIN DEFINITION #
########################

CC = gcc
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer
LDFLAGS = -m elf_i386

########################
##### MAKE PROCESS #####
########################

a.img: boot kernel
	dd if=boot of=a.img bs=512 count=1
	dd if=kernel of=a.img bs=512 seek=1

boot: boot.asm
	nasm boot.asm -o boot

kernel: kernel.out
	$(OBJCOPY) -O binary -j .text kernel.out kernel

kernel.out: head.o main.o
	$(LD) $(LDFLAGS) -N -e init -Ttext 0x00000000 -o kernel.out head.o main.o

head.o: head.asm
	nasm -f elf32 head.asm -o head.o

main.o: main.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -I. -c main.c

clean: 
	rm boot kernel kernel.out head.o main.o main.d

run: 
	bochs -q -f bochsrc

push: 
	git add -A
	git commit -m "snapshot: $(shell date)"
