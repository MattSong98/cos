#include "defs.h"

void 
syscall()
{
	if (cpu.proc->tf->eax == 0x10)
		cprintln("hello", TYPE_STR);

	if (cpu.proc->tf->eax == 0x20) {
		// read disk
		struct ide_buf *p = ide_bget(1, 0, IDE_RO);
		cprintf(&p->data[0], TYPE_HEX);
		ide_brelse(p);
	}

	if (cpu.proc->tf->eax == 0x30) {
		// write disk
		struct ide_buf *p = ide_bget(1, 2, IDE_RW);
		p->data[0] = 0xBB;
		ide_brelse(p);
		cprintln("done!", TYPE_STR);
	}

}
