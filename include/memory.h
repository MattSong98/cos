#ifndef _MEMORY_H
#define _MEMORY_H

// paging
#define LOW_MEM 1024*1024
#define PAGING_MEM 63*1024*1024
#define PAGING_PAGES (PAGING_MEM>>12)
#define RESERVED_MEM 15*1024*1024
#define RESERVED_PAGES (RESERVED_MEM>>12)
#define PAGE_SIZE 4096
#define USED 100

// kernel space selectors
#define CODE_SEL	0x08
#define DATA_SEL	0x10
#define SCRN_SEL	0x18

#endif
