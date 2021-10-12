#ifndef _PIC_H
#define _PIC_H

// pic ports
#define PIC1_COMD			0x20
#define PIC2_COMD			0xA0
#define PIC1_DATA			PIC1_COMD+1
#define PIC2_DATA			PIC2_COMD+1

#define PIC1_OFFSET		0x20
#define PIC2_OFFSET		0x28

#define PIC_EOI				0x20

// ibm irq convention
#define IRQ_TIMER			0
#define IRQ_KBD				1
#define IRQ_SLAVE			2
#define IRQ_FLOPPY		6
#define IRQ_SPU7			7
#define IRQ_IDE				14
#define IRQ_SPU15			15

#endif
