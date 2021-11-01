.text

start:
	mov $0x40, %eax		# wait
	int $0x30
	jmp start
