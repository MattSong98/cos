// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "console.h"
#include "defs.h"
#include "trap.h"
#include "x86.h"

int 
main(void) {

	mm_init();
	pic_init();
	init_cga();
	kbd_init();
	// sti();
	
	while(1);
	return 0;
}   
