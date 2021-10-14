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

static struct gate_desc idt[IDT_SIZE];

void 
idt_init()
{
	for (uchar i = 0; i < IDT_SIZE; i++) {
		SET_GATE(idt[i], vectors[i], CODE_SEL, INTERRUPT_GATE);	
	}
	SET_GATE(idt[T_SYSCALL], vectors[T_SYSCALL], CODE_SEL, TRAP_GATE);	
	lidt(idt, sizeof(idt));
}

void
trap(struct trapframe *tf) 
{
	if(tf->trapno == T_SYSCALL) {
		// do nothing for now
		return;
	}
	
	switch(tf->trapno) {
		case T_TIMER:
			break;

		case T_KBD:
			kbd_intr();
			pic_send_eoi(IRQ_KBD);
			break;
			
		case T_SPUR7:
			// shall not send eoi. once no device 
			// attaches to irq7, it's fine to leave 
			// it unhandled.
			break;

		case T_IDE:
			break;

		case T_SPUR15:
			// same as spurious irq on irq7
			break;

		default:
			// do nothing for now
	}		
}







