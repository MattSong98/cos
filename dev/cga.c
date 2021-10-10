#include "cga.h"

#define CGA_SIZE 80*25
#define CGA_ROW_SIZE 25
#define CGA_COL_SIZE 80
#define CGA_STD_ATR 0x07

extern void 
_copy_to_cga (unsigned char c, unsigned char atr, unsigned short pos);

static unsigned char buf[CGA_SIZE];
static unsigned char atr[CGA_SIZE];
static unsigned short pos;

static void write_char_to_buf(unsigned char);
static void scroll_up();
static void flush();

void init_cga() {
	for (unsigned short i = 0; i < CGA_SIZE; i++) {
		buf[i] = ' ';
		atr[i] = CGA_STD_ATR;
	}
	flush();
}

void write_cga(void *ptr, int type) {

	if (type == TYPE_HEX) {
		// assume sizeof(TYPE_HEX) == 4
		unsigned char digits[8];
		unsigned long hex = *(unsigned long *)(ptr);
		unsigned char dig;
		for (int i = 0; i < 8; i++) {
			dig = (unsigned char)(hex % 16); 
			if (dig < 10) dig += 48;
			else dig += 55;
			digits[i] = dig;
			hex = hex / 16;
		}
		write_char_to_buf('0');
		write_char_to_buf('x');
		for (int i = 7; i >= 0; i--) {
			write_char_to_buf(digits[i]);
		}
		flush();
	} 
}

void clear_cga() {
	pos = 0;
	for (unsigned short i = 0; i < CGA_SIZE; i++) {
		buf[i] = ' ';
	}
	flush();
}

void flush() {
	for (unsigned short i = 0; i < CGA_SIZE; i++) {
		_copy_to_cga(buf[i], atr[i], i);
	}
}

void write_char_to_buf(unsigned char c) {
	if (pos == CGA_SIZE) {
		scroll_up();
	}
	buf[pos++] = c;
}

void scroll_up() {
	for (unsigned short i = 0; i < CGA_SIZE - CGA_COL_SIZE; i++) {
		buf[i] = buf[i+CGA_COL_SIZE];
	}
	for (unsigned short i = CGA_SIZE - CGA_COL_SIZE; i < CGA_SIZE; i++) {
		buf[i] = ' ';
	}
	pos = CGA_SIZE - CGA_COL_SIZE;
}

