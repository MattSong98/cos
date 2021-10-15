// C entry

#include "defs.h"

int 
main(void) {

	// setup memory & trap	
	mm_init();
	trap_init();

	// setup i/o dev
	// console remains fixing
	// to-do: ide, timer, dma, nic
	pic_init();
	console_init();

	// enable interrupt
	sti();

	// spin	
	for(;;);	
	return 0;

}   
