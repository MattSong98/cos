#ifndef _TRAP_H
#define _TRAP_H

// type of gates 
#define INTERRUPT_GATE	0x8F00
#define TRAP_GATE				0xEF00

// idt size shall not be over 0x100
#define IDT_SIZE 0x100

// Attention! trap vectors shall be consistent to pic offset and irq 
// convention in include/pic.h
#define T_TIMER		0x20
#define T_KBD			0x21
#define T_SPUR7		0x27
#define T_IDE			0x2E
#define T_SPUR15	0x2F
#define T_SYSCALL 0x30

// x86 gate descriptor
struct gate_desc 
{
	uint off_15_0 : 16;
	uint cs : 16;
	uint type : 16;
	uint off_31_16 : 16;
};

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
