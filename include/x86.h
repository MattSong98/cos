#ifndef _X86_H
#define _X86_H

#include "types.h"

static inline uchar
inb(ushort port) {
	uchar data;
	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
outb(ushort port, uchar data) {
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

#endif
