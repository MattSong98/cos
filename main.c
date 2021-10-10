// main called by head.s
// from now on, we stay with C.

int num_data = 7;
int num_bss;

void _println(void);
void _printtb(void);
void _print_reg(unsigned);

unsigned int fact(unsigned);

// entry point here

int
main(void) {
	unsigned a = 10;
	_print_reg(a);
	_println();
	_print_reg(fact(a));
	_println();
	_print_reg(num_data);
	while (1);
	return 0;
}

unsigned fact(unsigned n) {
	if (n == 1) return 1;
	else return fact(n-1) * n;
}
