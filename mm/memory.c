// Kernel sticks below first 4 MiB.
// Note that VGA RAM and BIOS will remain in the space 
// from 640KM up to 1M.
// For the space upper 4M, it serves as the container 
// of pages for user space processes.
// Assume there is at least 1GiB space available.

#include "defs.h"

// gdt
struct seg_desc gdt[GDT_SIZE];

// page table should be aligned to a normal page
// cause cr3 does not make use of the low 12 bits.
__attribute__((__aligned__(NORM_PAGE_SIZE)))
struct pte kpgtab[PTES];

// allocable physical pages
static struct phypage *free_page_list;

// setup global descriptor table for both
// kernel space & user space.
// local descriptor table is not used.

static void 
set_segment(struct seg_desc *p, uint offset, uint limit, uint type) {
	p->off_15_0 = offset & 0xFFFF;
	p->off_23_16 = (offset >> 16) & 0x00FF;
	p->off_31_24 = offset >> 24;
	p->limit_15_0 = limit & 0xFFFF;
	p->limit_19_16 = limit >> 16;
	p->type_7_0 = type & 0xFF;
	p->type_11_8 = type >> 8;
}

void 
gdt_init()
{
	// null segment
	set_segment(&gdt[0], 0, 0, 0);
	// code segment
	set_segment(&gdt[1], CODE_SEG_OFFSET, CODE_SEG_LIMIT, CODE_SEG_TYPE);
	// data segment
	set_segment(&gdt[2], DATA_SEG_OFFSET, DATA_SEG_LIMIT, DATA_SEG_TYPE);
	// vram segment
	set_segment(&gdt[3], VRAM_SEG_OFFSET, VRAM_SEG_LIMIT, VRAM_SEG_TYPE);
	// tss segment
	set_segment(&gdt[4], TSS_SEG_OFFSET, TSS_SEG_LIMIT, TSS_SEG_TYPE);
	// user code segment
	set_segment(&gdt[5], UCODE_SEG_OFFSET, UCODE_SEG_LIMIT, UCODE_SEG_TYPE);
	// user data segment
	set_segment(&gdt[6], UDATA_SEG_OFFSET, UDATA_SEG_LIMIT, UDATA_SEG_TYPE);
	// load gdt	
	lgdt((uint)gdt, sizeof(gdt));
}

// setup kernel virtual memory for
// a given page table.
void 
kvm_setup(struct pte *pgtab)
{
	for (uint i = 0; i < KVM_PAGES; i++) {
		pgtab[i].pte_p = 1;
		pgtab[i].pte_w = 1;
		pgtab[i].pte_ps = 1;
		pgtab[i].pte_ad = (PAGE_SIZE >> 12) * i;
	}
}

void
kvm_init() 
{
	// initialize kpgdir
	kvm_setup(kpgtab);

	// apply to h/w
	asm volatile (	
		// turn on page size extension
		"movl %%cr4, %%eax\n\t"	
		"orl %1, %%eax\n\t"
		"movl %%eax, %%cr4\n\t"
								
		// set page directory
		"movl %0, %%cr3\n\t"
							
		// turn on paging
		"movl %%cr0, %%eax\n\t"
		"orl %2, %%eax\n\t"
		"movl %%eax, %%cr0\n\t"
		:: "r" (kpgtab), "i" (CR4_PSE), "i" (CR0_PG|CR0_WP)
		: "eax" );	
}

// page allocator which allocates physical
// pages started from 4MiB to PHYSTOP
void
pgalloc_init()
{
	// attach next free page's pa to the current
	// free page.
	// first phy page is reversed for kernel itself.
	for (uint i = 1; i < PHY_PAGES - 1; i++) {
		struct phypage *cur = (struct phypage *) (PAGE_SIZE * i);
		cur->next = (struct phypage *) (PAGE_SIZE * (i+1));
	}
	struct phypage *last = (struct phypage *) (PAGE_SIZE * (PHY_PAGES - 1));
	last->next = NULL;
	free_page_list = (struct phypage *) (PAGE_SIZE);
}

// free a phypage at the given pa.
void
page_free(uint pa)
{
	if (pa % PAGE_SIZE != 0)
		panic("page_free");
	
	if (pa == 0)
		panic("page_free");

	// wipe out all the content
	for (uint *p = (uint *) pa; p < (uint *) (pa + PAGE_SIZE); p++) 
		*p = 0;

	// add to free_page_list
	struct phypage *p = (struct phypage *) pa;
	p->next = free_page_list;
	free_page_list = p;
}

// alloc a phypage and return its pa.
// run out of pages if it returns 0.
uint
page_alloc(void)
{	
	if (free_page_list->next == NULL)
		return 0;

	uint pa = (uint) free_page_list;
	free_page_list = free_page_list->next;
	return pa;
}

