// C entry

#include "defs.h"

int
main(void) {

	console_init();
	
	kvm_init();
	pgalloc_init();

	pic_init();
	timer_init();
	ide_init();
	
	cprintf("hello", TYPE_STR);
	trap_init();
	// enable interrupt
	sti();

	// spin	
	for(;;);	
	return 0;

}   
