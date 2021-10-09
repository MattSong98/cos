// main called by head.s
// from now on, we stay with C.

void _println(void);
void _printtb(void);
void _print_reg(unsigned);

int
main(void) {
	_print_reg(0x100);
	_println();
	_print_reg(0x200);
	while (1);
	return 0;
}

void readelf() {

}
