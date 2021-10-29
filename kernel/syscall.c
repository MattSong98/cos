#include "defs.h"

void 
syscall()
{
	if (cpu.proc->tf->eax == 0x00)
		cprintln("hello", TYPE_STR);

	if (cpu.proc->tf->eax == 0x10)
		cprintln("bonjour", TYPE_STR);

	if (cpu.proc->tf->eax == 0x20) {
		// read disk
		struct ide_buf *p = ide_bget(1, 0, IDE_RO);
		cprintf(&p->data[0], TYPE_HEX);
		ide_brelse(p);
	}

	if (cpu.proc->tf->eax == 0x30) {
		static uint i = 0;
		if (i < 128) {
			cprintln("PROC 0", TYPE_STR);
			// cprintf("PROC: 0  i:", TYPE_STR);
			// cprintln(&i, TYPE_HEX);
			proc_dump();
			struct ide_buf *p = ide_bget(1, i++, IDE_RW);
			p->data[0] = 0x01;
			ide_brelse(p);
			return;
		} else {
			return;
		}
		
	}

	if (cpu.proc->tf->eax == 0x40) {
		static uint i = 0;
		if (i < 128) {
			cprintln("PROC 1", TYPE_STR);
			// cprintf("PROC: 0  i:", TYPE_STR);
			// cprintln(&i, TYPE_HEX);
			proc_dump();
			struct ide_buf *p = ide_bget(1, i++, IDE_RW);
			p->data[1] = 0x02;
			ide_brelse(p);
			return;
		} else {
			return;
		}
		
	}

	if (cpu.proc->tf->eax == 0x56) {
		static uint i = 0;
		if (i >= 4) return;
		cprintf("PROC: 1  i:", TYPE_STR);
		cprintf(&i, TYPE_HEX);
		proc_dump();
		struct ide_buf *p = ide_bget(1, i, IDE_RW);
		if (p == NULL) {
			cprintf(" busy ", TYPE_STR);
			i++;
			return;
		}
		cprintf("gotccha  ", TYPE_STR);
		p->data[1] = 0x22;
		ide_brelse(p);
		i++;
		cprintln("success", TYPE_STR);
		return;
		
	}

	if (cpu.proc->tf->eax == 0x4) {
		cprintln("0x40", TYPE_STR);
		proc_dump();
		static uint a[128];
		static uint i = 0;
		if (a[i % 128] == 0) {
			struct ide_buf *p = ide_bget(1, i, IDE_RW);
			if (p == NULL) {
				i++;
				return;
			}
			p->data[1] = 0x22;
			ide_brelse(p);
			a[i % 128] = 1;
			i++;
			return;
		}
	}
}
