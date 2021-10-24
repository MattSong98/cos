#ifndef _MEMORY_H
#define _MEMORY_H

#ifndef __ASSEMBLER__
struct pte {
	uint pte_p : 1; // present
	uint pte_w : 1;	// writeable
	uint pte_u : 1;	// user
	uint pte_pwt : 1;	// write through
	uint pte_pcd : 1; // cache disable
	uint pte_a : 1; // accessed
	uint pte_d : 1; // dirty
	uint pte_ps : 1; // page size
	uint pte_res : 4; // must be zero	
	uint pte_ad : 20;
};

struct phypage {
	struct phypage *next;
};
#endif

#define NULL 0

#define CR0_PE	(1<<0) 
#define CR0_WP	(1<<16)
#define CR0_PG	(1<<31)
#define CR4_PSE	(1<<4)

#define NORM_PAGE_SIZE (1<<12) // for align only
#define PAGE_SIZE (1<<22) // 4 MiB

#define PTE_SIZE	4	// PTE takes 4B
#define PTES	1024	// each pgtab has 1024 PTEs

#define PHY_PAGES	32	// PHY_PAGES <= KVM_PAGES
#define KVM_PAGES	256	// 0-1GiB
#define UVM_PAGES	768	// 1-4GiB

// kernel space selectors
#define CODE_SEL	0x08
#define DATA_SEL	0x10
#define SCRN_SEL	0x18

#endif
