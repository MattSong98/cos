#ifndef _TIMER_H
#define _TIMER_h

// ports
#define TIMER0_DATA	0x40	// channel 0, generate IRQ0
#define TIMER1_DATA	0x41	// channel 1, not used
#define TIMER2_DATA	0x42	// channel 2, for speaker, not used
#define TIMER_COMD 0x43	// write only

// freq
#define TIMER_FREQ	1193182	// base freq without dividers
#define TIMER_DIV(x)	((TIMER_FREQ+(x)/2)/(x))

// comd
#define TIMER_SEL0	0x00	// select channel 0
#define TIMER_SEL1	0x40	// select channel 1
#define TIMER_SEL2	0x80	// select channel 2

#define TIMER_LOBYTE	0x10
#define TIMER_HIBYTE	0x20
#define TIMER_BIBYTE	0x30	// access mode: lobyte first followed by hibyte

#define TIMER_MODE0	0x00
#define TIMER_MODE1 0x02
#define TIMER_MODE2 0x04	// rate generator
#define TIMER_MODE3	0x06
#define TIMER_MODE4	0x08
#define TIMER_MODE5	0x0A

#endif 
