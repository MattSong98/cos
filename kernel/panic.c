#include "defs.h"

void 
panic(const char *err) 
{
	cprintf(err, TYPE_STR);
	// spin
	for(;;);
}	
