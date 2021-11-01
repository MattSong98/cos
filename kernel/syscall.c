#include "defs.h"

void 
syscall()
{
	// sys_hello
	if (cpu.proc->tf->eax == 0x00)
		cprintln("hello", TYPE_STR);

	// sys_bonjour
	if (cpu.proc->tf->eax == 0x10)
		cprintln("bonjour", TYPE_STR);

	// sys_disk
	if (cpu.proc->tf->eax == 0x20) {
		for (int i = 0; i < 4; i++) {
			struct ide_buf *p = ide_bget(1, i, IDE_RW);
			p->data[0] = 0x11;
			p->data[1] = 0x11;
			ide_brelse(p);
		}
	}

	// sys_disk
	if (cpu.proc->tf->eax == 0x30) {
		for (int i = 0; i < 6; i++) {
			struct ide_buf *p = ide_bget(1, i, IDE_RW);
			p->data[1] = 0x99;
			p->data[2] = 0x99;
			ide_brelse(p);
		}
	}

	// sys_wait	
	if (cpu.proc->tf->eax == 0x40) {
		wait();
	}

	// sys_exit
	if (cpu.proc->tf->eax == 0x50) {
		cprintln("exit", TYPE_STR);
		exit();
	}
}
