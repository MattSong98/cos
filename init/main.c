// main called by head.s
// from now on, we stay with C.

#include "defs.h"

int 
main(void) {
	init_cga();
	char mesg1[] = "cga ready  ";
	write_cga(mesg1, TYPE_STR);
	
	mm_init();
	char mesg2[] = "mm ready  ";
	write_cga(mesg2, TYPE_STR);

	pic_init();
	char mesg3[] = "pic ready  ";
	write_cga(mesg3, TYPE_STR);

	kbd_init();
	char mesg4[] = "kbd ready  ";
	write_cga(mesg4, TYPE_STR);

	idt_init();
	char mesg5[] = "idt ready  ";
	write_cga(mesg5, TYPE_STR);
	
	while(1);
	return 0;
}   
