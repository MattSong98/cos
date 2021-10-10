// main called by head.s
// from now on, we stay with C.

#include "mm.h"

int num_data = 7;

void _println(void);
void _printtb(void);
void _print_reg(unsigned);

unsigned int fact(unsigned);

// entry point here

int
main(void) {
	_print_reg(PAGE_SIZE);
	_println();
	_print_reg(num_data);
	while (1);
	return 0;
}

unsigned fact(unsigned n) {
	if (n == 1) return 1;
	else return fact(n-1) * n;
}
