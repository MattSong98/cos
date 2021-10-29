.text

start:
	mov $0x30, %eax # when eax = 10, print out "hello, world".
	int $0x30
#	mov $0x20, %eax
#	int $0x30
spin:
	jmp spin
