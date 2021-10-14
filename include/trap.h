#ifndef _TRAP_H
#define _TRAP_H

// kernel space selectors
#define CODE_SEL	0x08
#define DATA_SEL	0x10
#define SCRN_SEL	0x18

// type of gates 
#define INTERRUPT_GATE	0x8F00
#define TRAP_GATE				0xEF00

// idt size shall not be over 0x100
#define IDT_SIZE 0x100

// trap vectors shall be consistent to pic offset and irq 
// convention in include/pic.h
#define T_TIMER		0x20
#define T_KBD			0x21
#define T_SPUR7		0x27
#define T_IDE			0x2E
#define T_SPUR15	0x2F
#define T_SYSCALL 0x30

#define SET_GATE(gate, off, sel, tag)		\
{																					\
	(gate).off_15_0 = (uint)(off) & 0xFFFF; \
	(gate).cs = (uint)(sel) & 0xFFFF; 			\
	(gate).type = (uint)(tag) & 0xFFFF;		\
	(gate).off_31_16 = (uint)(off) >> 16;		\
}																					

struct gate_desc 
{
	uint off_15_0 : 16;
	uint cs : 16;
	uint type : 16;
	uint off_31_16 : 16;
};

#endif 
