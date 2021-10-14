// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "console.h"
#include "defs.h"
#include "x86.h"

int 
main(void) {
	char mesg[] = "hello, there!";
	write_cga(mesg, TYPE_STR);
	mm_init();
	write_cga(mesg, TYPE_STR);
	pic_init();
	write_cga(mesg, TYPE_STR);
	init_cga();
	write_cga(mesg, TYPE_STR);
	kbd_init();
	write_cga(mesg, TYPE_STR);
	idt_init();
	write_cga(mesg, TYPE_STR);
	sti();
	
	while(1);
	return 0;
}   
