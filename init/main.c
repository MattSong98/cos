// C entry (from boot.asm)
// The environment should be correctly set 
// before ljumping to main:
// +---------------------------+
// | CS:CODE_SEL | EIP:0x00000 |
// | DS:DATA_SEL |						 |
// | SS:DATA_SEL | ESP:0xA0000 |
// +---------------------------+

#include "defs.h"

extern void gdt_test();

int
main(void) {

	gdt_init();
	console_init();

	// gdt_test();

//	panic("debug11");
	
	kvm_init();
	pgalloc_init();

	pic_init();
	timer_init();
	ide_init();
	
	trap_init();

	proc_init();
	user_init();

	scheduler();
}
