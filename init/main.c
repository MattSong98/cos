// main called by head.s
// from now on, we stay with C.

#include "mm.h"

void init_cga(void);
void write_cga(void *, int);

// entry point here

int
main(void) {
	
    init_cga();
	unsigned long a = 0x12345678;
	write_cga(&a, 0);
	while(1);	


	init_memory();

	
	while (1);
	return 0;
}
