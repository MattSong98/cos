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

// trap vectors from trapasm.S
extern uint vectors[];

// Layout of the trap frame built on the stack by the
// hardware and by trapasm.S, and passed to trap.c.
struct trapframe {
  // registers as pushed by pusha
  uint edi;
  uint esi;
  uint ebp;
  uint oesp;      // useless & ignored
  uint ebx;
  uint edx;
  uint ecx;
  uint eax;

  // rest of trap frame
  ushort gs;
  ushort padding1;
  ushort fs;
  ushort padding2;
  ushort es;
  ushort padding3;
  ushort ds;
  ushort padding4;
  uint trapno;

  // below here defined by x86 hardware
  uint err;
  uint eip;
  ushort cs;
  ushort padding5;
  uint eflags;

  // below here only when crossing rings, such as from user to kernel
  uint esp;
  ushort ss;
  ushort padding6;
};

#endif 
