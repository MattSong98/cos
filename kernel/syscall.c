#include "defs.h"



void 
syscall()
{
	if (cpu.proc->tf->eax == 0x10000) {
		panic("hello, i686-elf!");		
	}

}
