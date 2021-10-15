// It is important to note that APIC has replaced the 8259 PIC 
// in more modern system, especially those with multiple cores.
// we choose to setup PIC in a rather simple way, which conforms 
// to the configuration in early linux 0.11. 

#include "defs.h"

// Current irq mask
// Initial irq mask has irq2 enabled for slave pic
static ushort irqmask = 0xFFFF & ~(1<<IRQ_SLAVE);

// read/write mask with ocw1 
static void
pic_set_mask(ushort mask)
{
	irqmask = mask;
	outb(PIC1_DATA, mask);
	outb(PIC2_DATA, mask>>8);
}

// irq ranges from 0 ~ 15
void
pic_enable_irq(uchar irq)
{
	pic_set_mask(irqmask & ~(1<<irq));
}

// pic initailized without aeoi should send 
// eoi manually with ocw2
void 
pic_send_eoi(uchar irq)
{
	if (irq >= 8) 
		outb(PIC2_COMD, PIC_EOI);
	outb(PIC1_COMD, PIC_EOI);
}

// initialize pic to the illustrated simple mode
void 
pic_init(void) 
{
	// mask all interrupts
	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);

	// set up icw1
	outb(PIC1_COMD, 0x11);
	outb(PIC2_COMD, 0x11);

	// set up icw2
	outb(PIC1_DATA, PIC1_OFFSET);
	outb(PIC2_DATA, PIC2_OFFSET);

	// set up icw3
	outb(PIC1_DATA, 1<<IRQ_SLAVE);
	outb(PIC2_DATA, IRQ_SLAVE);

	// set up icw4
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

	// set up mask
	if (irqmask != 0xFFFF)
		pic_set_mask(irqmask);
}


