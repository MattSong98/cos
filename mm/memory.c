// Kernel sticks below 640KB and we reserve the PA
// from 1M - 16M for future usage. Note that VGA RAM 
// and BIOS will remain the space from 640KM up to 1M.
// We won't cast any modification here.
// For the space upper 1M, it serves as the container 
// for frames of user space processes.
// We assume there is at least 64M space available.

#include "defs.h"

static uchar mm_map[PAGING_PAGES];

void 
mm_init() 
{
	for (uint i = 0; i < RESERVED_PAGES; i++) 
		mm_map[i] = USED;	
}

// if success, return PA of the new Page.
// otherwise, return 0
uint
get_free_page() 
{
	for (uint i = RESERVED_PAGES; i < PAGING_PAGES; i++) {
		if (mm_map[i] != USED) {     
			mm_map[i] = USED;
			return LOW_MEM + (i<<12);
		}
	}
	return 0;
}

// free the page at given PA.
// beforing handling, check the validity of PA.
// if we sucess, return 0, otherwise, return -1.
int 
free_page(uint pa) 
{
	if (pa % PAGE_SIZE == 0 && pa >= LOW_MEM) {
		mm_map[(pa-LOW_MEM) >> 12] = 0;
		return 0;
	}
	return -1;
}

