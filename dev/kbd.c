#include "types.h"
#include "x86.h"
#include "kbd.h"

int
kbdgetc(void) {
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

