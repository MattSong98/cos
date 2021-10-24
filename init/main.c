// C entry

#include "defs.h"

int
main(void) {

	console_init();
	
	kvm_init();
	pgalloc_init();

	pic_init();
	timer_init();
	ide_init();
	
	trap_init();
	// enable interrupt
	sti();

	struct ide_buf *buf = ide_bget(1, 0, IDE_RW);
	cprintf("data: ", TYPE_STR);
	cprintf(&(buf->data[0]), TYPE_HEX);
	cprintf(&(buf->data[4]), TYPE_HEX);
	cprintf(&(buf->data[8]), TYPE_HEX);
	cprintf(&(buf->data[12]), TYPE_HEX);
	buf->data[0] = 0xFF;
	buf->data[1] = 0xFF;
	buf->data[2] = 0xFF;
	buf->data[3] = 0xFF;
	ide_brelse(buf);
	// spin	
	for(;;);	
	return 0;

}   
