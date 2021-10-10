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

cos.img: boot kernel
	dd if=boot of=cos.img bs=512 count=1
	dd if=kernel of=cos.img bs=512 seek=1

boot: boot.asm
	nasm boot.asm -o boot

kernel: kernel.out
	$(OBJCOPY) -O binary -j .text -j .data -j .bss kernel.out kernel

kernel.out: head.o main.o
	$(LD) $(LDFLAGS) -N -e init -Ttext 0x00000000 -o kernel.out head.o main.o

head.o: head.asm
	nasm -f elf32 head.asm -o head.o

main.o: main.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -I. -c main.c

########################
###### ULTILITIES ######
########################

clean: 
	rm boot kernel kernel.out head.o main.o main.d

run: pull
	bochs -q -f bochsrc

push: 
	git add -A
	git commit -m "snapshot-date:$(shell date)"
	git push origin master

pull: 
	git pull origin master

