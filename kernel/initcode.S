.text

start:
	mov $0x10000, %eax		# test
	int $0x30

spin:
	jmp spin
