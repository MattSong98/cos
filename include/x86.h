#ifndef _X86_H
#define _X86_H

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
lgdt(uint ad, ushort size)
{
	volatile ushort pd[3];

	pd[0] = size - 1;
	pd[1] = ad & 0xFFFF;
	pd[2] = ad >> 16;

	asm volatile("lgdt (%0)" : : "r" (pd));
}

static inline void 
lidt(uint ad, ushort size)
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

static inline void
cli(void)
{
	asm volatile("cli");
}

// mov dword from port to es:addr cnt times.
static inline void
insl(int port, void *addr, int cnt)
{
  asm volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

// mov dword from es:addr to port cnt times.
static inline void
outsl(int port, const void *addr, int cnt)
{
  asm volatile("cld; rep outsl" :
               "=S" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

#endif
