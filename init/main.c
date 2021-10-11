// main called by head.s
// from now on, we stay with C.

#include "types.h"
#include "defs.h"

int 
main(void) {

	init_memory();
    init_cga();
	init_kbd();
	while (1) {
		int c_int = kbdgetc();
		if (c_int > 0) {
			uchar c = (uchar) c_int;
			write_cga(&c, TYPE_CHAR);
		}
	} 
	
	while(1);
	return 0;
}   
