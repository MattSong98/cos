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

kernel: kernel.out
	$(OBJCOPY) -O binary -j .text kernel.out kernel

kernel.out: head.o main.o
	$(LD) $(LDFLAGS) -N -e init -Ttext 0x00000000 -o kernel.out head.o main.o

head.o: head.asm
	nasm -f elf32 head.asm -o head.o

main.o: main.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -I. -c main.c

clean: 
	rm kernel.out head.o main.o
