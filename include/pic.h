#ifndef _PIC_H
#define _PIC_H

// pic ports
#define PIC1_COMD			0x20
#define PIC2_COMD			0xA0
#define PIC1_DATA			PIC1_COMD+1
#define PIC2_DATA			PIC2_COMD+1

// make sure it's consistent to definiton 
// of trap vectors in include/trap.h
// note that: vector = offset + irq
#define PIC1_OFFSET		0x20
#define PIC2_OFFSET		PIC1_OFFSET+8

// ibm irq convention 
// no modification allowed 
#define IRQ_TIMER			0
#define IRQ_KBD				1
#define IRQ_SLAVE			2
#define IRQ_SPUR7			7
#define IRQ_IDE				14
#define IRQ_SPUR15		15

// end of interrupt signal
#define PIC_EOI				0x20

#endif
