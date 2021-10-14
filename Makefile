########################
# TOOLCHAIN DEFINITION #
########################

CC = gcc
AS = as
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer -fno-stack-protector 
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
LDFLAGS = -m elf_i386

########################
##### MAKE PROCESS #####
########################

target/cos.img: target/boot target/kernel
	dd if=target/boot of=target/cos.img bs=512 count=1
	dd if=target/kernel of=target/cos.img bs=512 seek=1

target/boot: init/boot.asm
	nasm init/boot.asm -o target/boot

target/kernel: target/kernel.out
	$(OBJCOPY) -O binary -j .text -j .data -j .bss -j .rodata --set-section-flags .bss=alloc,load,contents  target/kernel.out target/kernel

target/kernel.out: target/head.o target/main.o target/memory.o target/console.o target/kbd.o target/panic.o target/pic.o target/trap.o target/trapasm.o
	$(LD) $(LDFLAGS) -N -e init -Ttext 0x00000000 -o target/kernel.out target/head.o target/main.o target/memory.o target/console.o target/kbd.o target/panic.o target/pic.o target/trap.o target/trapasm.o

target/head.o: init/head.asm
	nasm -f elf32 init/head.asm -o target/head.o

target/main.o: init/main.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c init/main.c -o target/main.o

target/memory.o: mm/memory.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c mm/memory.c -o target/memory.o

target/console.o: dev/console.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c dev/console.c -o target/console.o

target/kbd.o: dev/kbd.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c dev/kbd.c -o target/kbd.o

target/panic.o: kernel/panic.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c kernel/panic.c -o target/panic.o

target/pic.o: dev/pic.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c dev/pic.c -o target/pic.o

target/trap.o: kernel/trap.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c kernel/trap.c -o target/trap.o

target/trapasm.o: kernel/trapasm.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c kernel/trapasm.S -o target/trapasm.o

########################
###### ULTILITIES ######
########################

clean: 
	rm -rf target/*

run: 
	bochs -q -f bochsrc

pullr: pull run 

push: 
	git add -A
	git commit -m "snapshot-date:$(shell date)"
	git push origin master

pull: 
	git pull origin master

