// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "mm.h"
#include "cga.h"
#include "keyboard.h"
#include "panic.h"

extern void read_scan_code();

int
main(void) {

	init_memory();
    init_cga();
	init_keyboard();
	read_scan_code();
	
	while(1);
	return 0;
}
