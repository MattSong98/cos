// Intel 8253/8254/82C54 Programmable Interval Timer (PIT).
// Only used on uniprocessors;
// SMP machines use the local APIC timer.


#include "defs.h"


//--------------------------
//
//   function : init    
//
//--------------------------


void
timer_init(void)
{
	// Interrupt 100 times/sec. (20 times/ sec)
	outb(TIMER_COMD, TIMER_SEL0 | TIMER_BIBYTE | TIMER_MODE2); 
	outb(TIMER0_DATA, TIMER_DIV(20) % 256);
	outb(TIMER0_DATA, TIMER_DIV(20) / 256);
	pic_enable_irq(IRQ_TIMER);
}
