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


#include "defs.h"

static uchar shiftcode[256] =
{
  [0x1D] CTL,
  [0x2A] SHIFT,
  [0x36] SHIFT
};

static uchar normalmap[256] =
{
  NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
  'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
  '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  // 0x30
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
};

static uchar shiftmap[256] =
{
  NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
  'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
  '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  // 0x30
  NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
  NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
};

static uchar ctlmap[256] =
{
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
  NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
  C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
  C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
  C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
  NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
  C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
};

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
	static uchar shift;
	static uchar *charcode[4] = {
		normalmap, shiftmap, ctlmap, ctlmap
	};
	uchar data, c;

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
	console_intr(kbdgetc());
}


