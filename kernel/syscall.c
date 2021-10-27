#include "defs.h"

void 
syscall()
{
	if (cpu.proc->tf->eax == 0x10)
		cprintf("hello", TYPE_STR);
	if (cpu.proc->tf->eax == 0x20)
		cprintf("bonjour", TYPE_STR);
}
