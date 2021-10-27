#include "defs.h"

void 
panic(const char *err) 
{
	cprintln(err, TYPE_STR);
	// spin
	for(;;);
}	
