// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "mm.h"
#include "cga.h"


int
main(void) {

	init_memory();
    init_cga();
	char hello[] = "hello";
	write_cga(hello, TYPE_STR);

	while(1);
	return 0;
}
