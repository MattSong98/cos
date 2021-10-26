// C entry

#include "defs.h"

int
main(void) {
	gdt_init();
	console_init();
	
	kvm_init();
	pgalloc_init();

	pic_init();
	timer_init();
	ide_init();
	
	trap_init();

	proc_init();
	user_init();

	scheduler();
}
