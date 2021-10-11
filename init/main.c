// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "mm.h"
#include "cga.h"
#include "keyboard.h"
#include "panic.h"

int
main(void) {

	init_memory();
    init_cga();
	init_keyboard();
	
	while(1);
	return 0;
}
