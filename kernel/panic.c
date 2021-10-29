#include "defs.h"

void 
panic(char *err) 
{
	cprintln(err, TYPE_STR);
	// spin
	for(;;);
}	
