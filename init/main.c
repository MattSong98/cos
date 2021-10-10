// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "mm.h"
#include "cga.h"
extern void write_cga_test();


int
main(void) {

	init_memory();
    init_cga();
	write_cga_test('0', 80);

	while(1);
	return 0;
}
