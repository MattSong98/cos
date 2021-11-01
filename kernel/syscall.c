#include "defs.h"

void 
syscall()
{
	if (cpu.proc->tf->eax == 0x00)
		cprintln("hello", TYPE_STR);

	if (cpu.proc->tf->eax == 0x10)
		cprintln("bonjour", TYPE_STR);

	if (cpu.proc->tf->eax == 0x30) {
		for (int i = 0; i < 128; i++) {
			struct ide_buf *p = ide_bget(1, i, IDE_RW);
			p->data[0] = 0x33;
			p->data[1] = 0x33;
			ide_brelse(p);
		}
	}

	if (cpu.proc->tf->eax == 0x40) {
		for (int i = 0; i < 128; i++) {
			struct ide_buf *p = ide_bget(1, i, IDE_RW);
			p->data[1] = 0x44;
			p->data[2] = 0x44;
			ide_brelse(p);
		}
	}

	if (cpu.proc->tf->eax == 0x50) {
		for (int i = 0; i < 128; i++) {
			struct ide_buf *p = ide_bget(1, i, IDE_RW);
			p->data[0] = 0xFF;
			p->data[1] = 0xFF;
			ide_brelse(p);
		}
	}
}
