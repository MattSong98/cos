// Attension, most but not all of keys are mapped.
// Unexpected results might produce due to unmapped keys.
// To keep it simple, state of kbd is captured by variable 
// "shift", which only have four valid values: NORMAL (0b0000), 
// SHIFT (0b0001), CONTROL (0b0010) and SHIFT & CONTROL (0b0011).

// Below shows valid mapped keys (which are capsuled by []), 
// take the US Keymap in a normal mode for example :) 

// [esc] f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 
// [`] [1] [2] [3] [4] [5] [6] [7] [8] [9] [0] [-] [=] [/b]
// [/t] [q] [w] [e] [r] [t] [y] [u] [i] [o] [p] [[] []] [\]
// capslock [a] [s] [d] [f] [g] [h] [j] [k] [l] [;] ['] [\n]
// [shift] [z] [x] [c] [v] [b] [n] [m] [,] [.] [/] [shift]
// fn [ctl] alt command [ ] command alt 


#include "types.h"
#include "kbd.h"
#include "x86.h"
#include "defs.h"
#include "pic.h"
#include "console.h"


static void 
ps2_init() 
{
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
	
	// Perform Interface Tests (only for the first port)
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xAB);
	while ((inb(0x64) & 0x01) != 1);
	if (inb(0x60) != 0x00) panic();

	// Enable Device (only for the first port)
	while ((inb(0x64) & 0x02) != 0);
	outb(0x64, 0xAE);
}

void 
kbd_init(void) 
{
	ps2_init();	
	pic_enable_irq(IRQ_KBD);
}

int 
kbdgetc(void) 
{
	static uint shift;
	static uchar *charcode[4] = {
		normalmap, shiftmap, ctlmap, ctlmap
	};
	uint data, c;

	if((inb(0x64) & 0x01) == 0) return -1;
	data = inb(0x60);

	if(data & 0x80) {
		// Key released
		data = data & 0x7F;
		shift &= ~(shiftcode[data]);
		return 0;
	}

	shift |= shiftcode[data];
	c = charcode[shift & (CTL | SHIFT)][data];
	return c;
}

void 
kbd_intr()
{
	int ret = kbdgetc();
	if (ret > 0) {
		uchar c = (uchar) ret;
		write_cga(&c, TYPE_CHAR);
	}
}


