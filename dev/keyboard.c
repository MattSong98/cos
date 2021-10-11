#include "keyboard.h"
#include "x86.h"
#include "cga.h"


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
	uchar test_res = inb(0x60);
	uint hex = (uint) test_res;
	write_cga(&hex, TYPE_HEX);

	// Dertermine if there are two channels; Skip Over
	// Perform Interface Tests

	// Enable Devices

	// Reset Devices
}


