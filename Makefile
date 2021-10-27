########################
# TOOLCHAIN DEFINITION #
########################

TOOLPREFIX = x86_64-elf-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer -fno-stack-protector 
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
LDFLAGS = -m elf_i386

########################
##### MAKE PROCESS #####
########################

cos.img: target/boot target/kernel
	dd if=target/boot of=cos.img bs=512 count=1
	dd if=target/kernel of=cos.img bs=512 seek=1

target/boot: init/boot.asm
	nasm init/boot.asm -o target/boot

target/kernel: target/kernel.out
	$(OBJCOPY) -O binary -j .text -j .data -j .bss -j .rodata --set-section-flags .bss=alloc,load,contents  target/kernel.out target/kernel

target/kernel.out: target/main.o target/memory.o target/console.o target/kbd.o target/panic.o target/pic.o target/trap.o target/trapasm.o target/timer.o target/ide.o target/proc.o target/string.o target/initcode target/swtch.o
	$(LD) $(LDFLAGS) -N -e main -Ttext 0x00000000 -o target/kernel.out target/main.o target/memory.o target/console.o target/kbd.o target/panic.o target/pic.o target/trap.o target/trapasm.o target/timer.o target/ide.o target/proc.o target/string.o target/swtch.o -b binary target/initcode 

target/main.o: init/main.c
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -Iinclude -c init/main.c -o target/main.o

target/memory.o: mm/memory.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c mm/memory.c -o target/memory.o

target/proc.o: kernel/proc.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c kernel/proc.c -o target/proc.o

target/console.o: dev/console.c
	$(CC) $(CFLAGS) -fno-pic -O0 -nostdinc -Iinclude -c dev/console.c -o target/console.o

target/kbd.o: dev/kbd.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c dev/kbd.c -o target/kbd.o

target/panic.o: kernel/panic.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c kernel/panic.c -o target/panic.o

target/pic.o: dev/pic.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c dev/pic.c -o target/pic.o

target/trap.o: kernel/trap.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c kernel/trap.c -o target/trap.o

target/trapasm.o: kernel/trapasm.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c kernel/trapasm.S -o target/trapasm.o

target/timer.o: dev/timer.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c dev/timer.c -o target/timer.o

target/ide.o: dev/ide.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c dev/ide.c -o target/ide.o

target/string.o: lib/string.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c lib/string.c -o target/string.o

target/swtch.o: kernel/swtch.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -Iinclude -c kernel/swtch.S -o target/swtch.o

target/initcode: kernel/initcode.S
	$(CC) $(CFLAGS) -nostdinc -Iinclude -c kernel/initcode.S -o target/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o target/initcode.out target/initcode.o
	$(OBJCOPY) -S -O binary target/initcode.out target/initcode

########################
###### ULTILITIES ######
########################

clean: 
	rm -rf target/*

run: 
	bochs -q -f bochsrc

push: 
	git add -A
	git commit -m "snapshot-date:$(shell date)"
	git push origin master

pull: 
	git pull origin master

