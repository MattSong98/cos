.text

start:
	mov $0x40, %eax		# print bonjour
	int $0x30

spin:
	jmp spin
