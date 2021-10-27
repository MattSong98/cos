.text

start:
	mov $0x10, %eax # when eax = 10, print out "hello, world".
	#int 0x30

spin:
	jmp spin
