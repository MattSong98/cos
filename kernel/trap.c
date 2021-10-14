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


#include "types.h"
#include "trap.h"
#include "x86.h"
#include "pic.h"
#include "defs.h"
#include "console.h"

static struct gate_desc idt[IDT_SIZE];

void 
idt_init()
{
	for (ushort i = 0; i < IDT_SIZE; i++) {
		SET_GATE(idt[i], vectors[i], CODE_SEL, INTERRUPT_GATE);	
	}
	SET_GATE(idt[T_SYSCALL], vectors[T_SYSCALL], CODE_SEL, TRAP_GATE);	
	lidt(idt, sizeof(idt));
	uint size = sizeof(idt);
	write_cga(&size, TYPE_HEX);
}

void
trap(struct trapframe *tf) 
{
	char mesg[] = "trapping...";
	write_cga(mesg, TYPE_STR);


	if(tf->trapno == T_SYSCALL) {
		// do nothing for now
		return;
	}
	
			uint no = tf->trapno;
			write_cga(&no, TYPE_HEX);
			uint nkbd = T_KBD;
			write_cga(&nkbd, TYPE_HEX);

	if (tf->trapno == T_KBD) {
			char mesg1[] = "kbd irq exec";
			write_cga(mesg1, TYPE_STR);
			kbd_intr();
			pic_send_eoi(T_KBD);
			
	} else {
			char mesg2[] = "unknown exec";
			write_cga(mesg2, TYPE_STR);
			
	}

			// do nothing for now
	
}







