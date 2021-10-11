#include "kbd.h"
#include "x86.h"
#include "cga.h"
#include "panic.h"
#include "types.h"

static void init_ps2_ctlr();

void init_keyboard() {
	init_ps2_ctlr();
}

void read_scan_code() {
	while(1) {
		while ((inb(0x64) & 0x01) != 1);
		uchar data = inb(0x60);
		uint hex = (uint) data;
		write_cga(&hex, TYPE_HEX);
	}
}

int kbdgetc(void) {
  	static uint shift;
  	static uchar *charcode[4] = {
    	normalmap, shiftmap, ctlmap, ctlmap
 	};
  	uint st, data, c;

  	st = inb(KBSTATP);
  	if((st & KBS_DIB) == 0) return -1;
  	data = inb(KBDATAP);

	if(shift & E0ESC) {
    	// Last character was an E0 escape; 
    	shift &= ~E0ESC;
		return 0;
	} else if(data == 0xE0) {
    	shift |= E0ESC;
    	return 0;
  	} else if(data & 0x80) {
    	// Key released
    	data = data & 0x7F;
    	shift &= ~(shiftcode[data]);
    	return 0;
	}

  	shift |= shiftcode[data];
  	c = charcode[shift & (CTL | SHIFT)][data];
  	return c;
}

void init_ps2_ctlr() {
	// initialize USB Controller: Skip Over
	// Determine if the PS/2 Controller exists : Skip Over
	// Disable both devices:
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xAD);
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xA7);

	// Flush the Output Buffer
	inb(0x60);

	// Set the Controller Configuration Byte
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0x20);
	while ((inb(0x64) & 0x01) != 1);
	uchar ccb = inb(0x60);
	ccb = ccb | 0x51;
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0x60);
	while ((inb(0x64) & 0x02) != 0);
	outb(0x60, ccb);

	// Perform Controller Self Test
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xAA);
	while ((inb(0x64) & 0x01) != 1);
	if (inb(0x60) != 0x55) panic();
	
	// Dertermine if there are two channels: Skip Over
	// Perform Interface Tests (only for the first port)
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xAB);
	while ((inb(0x64) & 0x01) != 1);
	if (inb(0x60) != 0x00) panic();

	// Enable Device (only for the first port)
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xAE);

	// Reset Devices: Skip Over
}


