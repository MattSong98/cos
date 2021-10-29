#include "defs.h"


//--------------------------
//
//    global variables
//
//--------------------------


/* shared */

static struct cga {
	uchar buf[CGA_SIZE];
	uchar atr[CGA_SIZE];
	uint pos;
} cga;


//--------------------------
//
//    function : init
//
//--------------------------


static void 
flush() 
{
	for (uint pos = 0; pos < CGA_SIZE; pos++) {
		ushort word = (ushort) cga.buf[pos] + (((ushort) cga.atr[pos]) << 8);
		asm volatile (
			"shl $1, %1\n\t"
			"addl %2, %1\n\t"
			"movw %0, (%1)\n\t" ::
		  "r" (word), "D" (pos), "i" (VRAM_BASE));
	}
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uchar) (cga.pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uchar) ((cga.pos >> 8) & 0xFF));
}


void 
lconsole_init() 
{
	cga.pos = 0;
	for (ushort i = 0; i < CGA_SIZE; i++) {
		cga.buf[i] = ' ';
		cga.atr[i] = CGA_STD_ATR;
	}
	flush();
}


//--------------------------
//
//   function : critical
//
//--------------------------


static void 
scroll_up() 
{
	for (ushort i = 0; i < CGA_SIZE - CGA_COL_SIZE; i++) {
		cga.buf[i] = cga.buf[i+CGA_COL_SIZE];
	}
	for (ushort i = CGA_SIZE - CGA_COL_SIZE; i < CGA_SIZE; i++) {
		cga.buf[i] = ' ';
	}
	cga.pos = CGA_SIZE - CGA_COL_SIZE - 1;
}


static void 
buf_write(uchar c) 
{
	if (cga.pos == CGA_SIZE - 1) {
		scroll_up();
	}
	cga.buf[cga.pos++] = c;
}


void 
lprintf(void *ptr, int type) 
{
	if (type == TYPE_HEX) {
		// assume sizeof(TYPE_HEX) == 4
		uchar digits[8];
		uint hex = *(uint *)(ptr);
		uchar dig;
		for (uchar i = 0; i < 8; i++) {
			dig = (uchar)(hex % 16); 
			if (dig < 10) dig += 48;
			else dig += 55;
			digits[i] = dig;
			hex = hex / 16;
		}
		buf_write('0');
		buf_write('x');
		for (uchar i = 0; i < 8; i++) {
			buf_write(digits[7-i]);
		}
		flush();
		return;
	} 

	if (type == TYPE_STR) {
		uchar *pchar = (uchar *)ptr;
		while (*pchar != '\0') {
			buf_write(*(pchar++));
		}
		flush();
		return;
	}

	if (type == TYPE_CHAR) {
		uchar c = *(uchar *)ptr;
		buf_write(c);
		flush();
		return;
	}
}


void 
lprintln(void *ptr, int type) 
{
	cprintf(ptr, type);
	while (cga.pos % CGA_COL_SIZE != 0)
		buf_write(' ');
	flush();
}


//--------------------------
//
//   function : intr 
//
//--------------------------


void 
lconsole_intr(int c) 
{
	if (c == -1) panic("console_intr");
	if (c == 0) return;
	
	cprintf(&c, TYPE_CHAR);
}

