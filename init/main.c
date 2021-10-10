// main called by head.s
// from now on, we stay with C.

#include "mm.h"
#include "cga.h"

int
main(void) {

	init_memory();
    init_cga();

	while(1);
	return 0;
}
