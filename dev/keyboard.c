#include "keyboard.h"
#include "x86.h"
#include "cga.h"
#include "panic.h"

static void init_ps2_ctlr();

void init_keyboard() {
	init_ps2_ctlr();
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


