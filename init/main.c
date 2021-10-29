// C entry: Coming from boot.asm, the environment should be initially
// set up for later formal initialization.
// => First, except for following requirements nothing shall be pre-configured.
// => Second, kernel (not greater than 512 KiB) should be moved down to PA:0.
// => Third, protected mode should have been enabled.
// => Fourth, segment (CODE, DATA) should set to offset:0 and limit:max.
// => Fifth, cs shoul be loaded with the selector of segment 'CODE', similaryly ds & ss 
// should be loaded with the selector of segment 'DATA'. Besides, eip should be set to 0 
// while esp should be set to 0xA0000.
// +-----------------------+
// | CS:CODE | EIP:0x00000 |
// | DS:DATA |						 |
// | SS:DATA | ESP:0xA0000 |
// +-----------------------+

#include "defs.h"

int
main(void) {
	
	console_init();
	lconsole_init();
	
	gdt_init();
	idt_init();
	kvm_init();
	cpu_init();

	pic_init();	
	timer_init();
	kbd_init();
	ide_init();

	palloc_init();
	proc_init();
	user_init();
	scheduler();
}



