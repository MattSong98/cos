.text

start:
	mov eax, 0x10	// when eax = 10, print out "hello, world".
	int 0x30
spin:
	jmp spin
