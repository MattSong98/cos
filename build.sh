nasm boot.asm -o .boot.bin
nasm head.asm -o .head.bin
dd if=.boot.bin of=a.img bs=512 count=1
dd if=.head.bin of=a.img bs=512 seek=1
