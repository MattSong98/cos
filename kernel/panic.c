#include "defs.h"

void 
panic(char *err) 
{
	cprintln(err, TYPE_STR);
	// spin
	for(;;);
}

void
pannic(char *err, uint n)
{
	static uint count = 0;
	if (count++	== n) {
		cprintln(err, TYPE_STR);
		for(;;);
	}
}
