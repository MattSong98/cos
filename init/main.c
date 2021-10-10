// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "mm.h"
#include "cga.h"

extern void init_ps2_ctlr();

int
main(void) {

	init_memory();
    init_cga();
	init_ps2_ctlr();
	
	while(1);
	return 0;
}
