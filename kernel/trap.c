// There are four types of gates in x86, namely interrupt gate, 
// trap gate, call gate & task gate. However we will only apply 
// the first two in the entire design of cos. 
// Interrupt gates differ from trap gates only in the aspect that 
// when getting trapped by interrupt gates, x86 will clear IF flag
// automatically in comparison to the scenario invoked by trap gates, 
// where x86 just keeps IF flag as what it was before.
// Traps in x86 can be classified into two categories, exceptions & 
// interrupts (including syscall). Exceptions can be trigerred
// by both hardware (eg. page fault, divide zero, etc.) and software
// (when x86 executing specific bound, into, int instructions), but 
// the timing is the same when cpu executing the faulty or invocative
// instruction. It's quite different from interrupts which always 
// trigerred by i/o device via a pic chip or instruction "int 0x80"
// (0x40 is the syscall vector chosed by cos). 


#include "defs.h"

static struct gate_desc idt[IDT_SIZE];

static void 
set_gate(struct gate_desc *p, uint offset, ushort cs, ushort type) {
	p->off_15_0 = offset & 0xFFFF;
	p->off_31_16 = offset >> 16;
	p->cs = cs;
	p->type = type;
}

void 
trap_init()
{
	for (ushort i = 0; i < IDT_SIZE; i++) {
		set_gate(idt+i, vectors[i], CODE_SEL, INTERRUPT_GATE);	
	}
	set_gate(idt+T_SYSCALL, vectors[T_SYSCALL], CODE_SEL, TRAP_GATE);	
	lidt((uint)idt, sizeof(idt));
}

void
trap(struct trapframe *tf) 
{
	if(tf->trapno == T_SYSCALL) {
		// do nothing for now
		return;
	}

	switch (tf->trapno) {
		case T_TIMER:
			break;
		case T_KBD:
			kbd_intr();
			pic_send_eoi(IRQ_KBD);
			break;
		case T_SPUR7:
			break;
		case T_IDE:
			break;
		case T_SPUR15:
			break;
		default:
			// unknown
			panic();
	}
			
}







