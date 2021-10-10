// main called by head.s
// from now on, we stay with C.

#include "mm.h"

void _println(void);
void _printtb(void);
void _print_reg(unsigned);

// entry point here

int
main(void) {
	
	_print_reg(sizeof(int));
	_println();

	init_memory();

	unsigned long ad1 = get_free_page();
	_print_reg(ad1);
	_println();

	unsigned long ad2 = get_free_page();
	_print_reg(ad2);
	_println();

	free_page(ad1);

	unsigned long ad3 = get_free_page();
	_print_reg(ad3);
	_println();
	
	while (1);
	return 0;
}
