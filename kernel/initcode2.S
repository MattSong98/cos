.text

start:
	mov $0x30, %eax		# disk 
	int $0x30

	mov $0x50, %eax		# exit
	int $0x30
	
spin:
	jmp spin
