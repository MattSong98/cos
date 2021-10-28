#include "defs.h"

void 
syscall()
{
	if (cpu.proc->tf->eax == 0x10)
		cprintln("hello", TYPE_STR);
	//if (cpu.proc->tf->eax == 0x20)
		//cprintln("bonjour", TYPE_STR);
}
