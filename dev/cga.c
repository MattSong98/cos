#include "types.h"
#include "cga.h"
#include "x86.h"

#define CGA_SIZE 80*25
#define CGA_ROW_SIZE 25
#define CGA_COL_SIZE 80
#define CGA_STD_ATR 0x07

extern void 
_copy_to_cga (uchar c, uchar atr, ushort pos);

static uchar buf[CGA_SIZE];
static uchar atr[CGA_SIZE];
static ushort pos;

// static void update_cursor(ushort);
static void write_char_to_buf(uchar);
static void scroll_up();
static void flush();

void hello() {
	write_char_to_buf('H');
	write_char_to_buf('e');
	write_char_to_buf('l');
	write_char_to_buf('l');
	write_char_to_buf('o');
	flush();
}

void init_cga() {
	for (ushort i = 0; i < CGA_SIZE; i++) {
		buf[i] = ' ';
		atr[i] = CGA_STD_ATR;
	}
	flush();
}

void write_cga(void *ptr, int type) {

	if (type == TYPE_HEX) {
		// assume sizeof(TYPE_HEX) == 4
		uchar digits[8];
		uint hex = *(unsigned int *)(ptr);
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
		for (uchar i = 7; i >= 0; i--) {
			write_char_to_buf(digits[i]);
		}
		flush();
	} 
}

void clear_cga() {
	pos = 0;
	for (ushort i = 0; i < CGA_SIZE; i++) {
		buf[i] = ' ';
	}
	flush();
}

void update_cursor(ushort pos) {
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uchar) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uchar) ((pos >> 8) & 0xFF));
}

void flush() {
	for (ushort i = 0; i < CGA_SIZE; i++) {
		_copy_to_cga(buf[i], atr[i], i);
	}
//	update_cursor(pos);
}

void write_char_to_buf(uchar c) {
	if (pos == CGA_SIZE) {
		scroll_up();
	}
	buf[pos++] = c;
}

void scroll_up() {
	for (ushort i = 0; i < CGA_SIZE - CGA_COL_SIZE; i++) {
		buf[i] = buf[i+CGA_COL_SIZE];
	}
	for (ushort i = CGA_SIZE - CGA_COL_SIZE; i < CGA_SIZE; i++) {
		buf[i] = ' ';
	}
	pos = CGA_SIZE - CGA_COL_SIZE;
}

