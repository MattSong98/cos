#include "panic.h"
#include "cga.h"
#include "types.h"

void panic() {
	uchar mesg[] = "Kernel Panic!";
	write_cga(mesg, TYPE_STR);
	for(;;);
}	
