#include "keyboard.h"
#include "x86.h"
#include "cga.h"


void init_ps2_ctlr() {
	
	// initialize USB Controller: Skip Over
		
	// Determine if the PS/2 Controller exists : Skip Over

	// Disable devices: 
		outb(0x64, 0xAD);
		outb(0x64, 0xA7);

	// Flush the Output Buffer
		do {
			uchar status = inb(0x64);
			status = status & 0x01;
			if (status == 1) {
				uchar data = inb(0x60);
				uint hex = (uint) data;
				write_cga(&data, TYPE_CHAR);
				write_cga(&hex, TYPE_HEX);
			}
		} while (1);

	// Set the Controller Configuration Byte

	// Perform Controller Self Test

	// Dertermine if there are two channels

	// Perform Interface Tests

	// Enable Devices

	// Reset Devices
}


