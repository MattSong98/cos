// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "mm.h"
#include "cga.h"

int
main(void) {

	init_memory();
    init_cga();
	uint a = 0x12345678;
	write_cga(&a, 0);

	while(1);
	return 0;
}
