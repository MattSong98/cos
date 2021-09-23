nasm boot.asm -o .boot.bin
nasm system.asm -o .system.bin
dd if=.boot.bin of=a.img bs=512 count=1
dd if=.system.bin of=a.img bs=512 seek=1
