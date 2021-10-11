#include "types.h"
#include "console.h"
#include "defs.h"

void panic() {
	uchar mesg[] = "Kernel Panic!";
	write_cga(mesg, TYPE_STR);
	for(;;);
}	
