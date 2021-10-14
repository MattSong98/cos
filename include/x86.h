#ifndef _X86_H
#define _X86_H

struct gate_desc 
{
	uint off_15_0 : 16;
	uint cs : 16;
	uint type : 16;
	uint off_31_16 : 16;
};

static inline uchar 
inb(ushort port) 
{
	uchar data;
	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
outb(ushort port, uchar data) 
{
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void 
lidt(uint ad, int size)
{
	volatile ushort pd[3];

	pd[0] = size - 1;
	pd[1] = ad & 0xFFFF;
	pd[2] = ad >> 16;

	asm volatile("lidt (%0)" : : "r" (pd));
}

static inline void
sti(void)
{
	asm volatile("sti");
}

#endif
