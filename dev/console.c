#include "types.h"
#include "defs.h"
#include "x86.h"
#include "console.h"

static uchar buf[CGA_SIZE];
static uchar atr[CGA_SIZE];
static ushort pos;

static void 
update_cursor(ushort pos) 
{
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uchar) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uchar) ((pos >> 8) & 0xFF));
}

static void 
flush() 
{
	for (ushort i = 0; i < CGA_SIZE; i++) {
		_copy_to_cga(buf[i], atr[i], i);
	}
	update_cursor(pos);
}

static void scroll_up() 
{
	for (ushort i = 0; i < CGA_SIZE - CGA_COL_SIZE; i++) {
		buf[i] = buf[i+CGA_COL_SIZE];
	}
	for (ushort i = CGA_SIZE - CGA_COL_SIZE; i < CGA_SIZE; i++) {
		buf[i] = ' ';
	}
	pos = CGA_SIZE - CGA_COL_SIZE - 1;
}

static void 
write_char_to_buf(uchar c) 
{
	if (pos == CGA_SIZE - 1) {
		scroll_up();
	}
	buf[pos++] = c;
}

void 
init_cga() 
{
	pos = 0;
	for (ushort i = 0; i < CGA_SIZE; i++) {
		buf[i] = ' ';
		atr[i] = CGA_STD_ATR;
	}
	flush();
}

void 
write_cga(void *ptr, int type) 
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
		write_char_to_buf('0');
		write_char_to_buf('x');
		for (uchar i = 0; i < 8; i++) {
			write_char_to_buf(digits[7-i]);
		}
		flush();
		return;
	} 

	if (type == TYPE_STR) {
		uchar *pchar = (uchar *)ptr;
		while (*pchar != '\0') {
			write_char_to_buf(*(pchar++));
		}
		flush();
		return;
	}

	if (type == TYPE_CHAR) {
		uchar c = *(uchar *)ptr;
		write_char_to_buf(c);
		flush();
		return;
	}
}

void 
clear_cga() 
{
	pos = 0;
	for (ushort i = 0; i < CGA_SIZE; i++) {
		buf[i] = ' ';
	}
	flush();
}
