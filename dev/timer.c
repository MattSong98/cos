// Intel 8253/8254/82C54 Programmable Interval Timer (PIT).
// Only used on uniprocessors;
// SMP machines use the local APIC timer.

#include "defs.h"

void
timer_init(void)
{
	// Interrupt 100 times/sec.
	outb(TIMER_COMD, TIMER_SEL0 | TIMER_BIBYTE | TIMER_MODE2); 
	outb(TIMER0_DATA, TIMER_DIV(100) % 256);
	outb(TIMER0_DATA, TIMER_DIV(100) / 256);
	pic_enable_irq(IRQ_TIMER);
}
