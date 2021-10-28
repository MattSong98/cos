// Kernel sticks below first 4 MiB.
// Note that VGA RAM and BIOS will remain in the space 
// from 640KM up to 1M.
// For the space upper 4M, it serves as the container 
// of pages for user space processes.
// Assume there is at least 1GiB space available.


#include "defs.h"


//--------------------------
//
//    global variables
//
//--------------------------


/* read only */

struct seg_desc gdt[GDT_SIZE];


/* read only */

// page table should be aligned to the boundary of a normal page
// cause cr3 does not make use of the low 12 bits.

__attribute__((__aligned__(NORM_PAGE_SIZE)))
struct pte kpgtab[PTES];


/* shared */

// allocable physical pages

static struct {
	lock *lock;
	struct phypage *head;
} plist;


//--------------------------
//
//     function : init
//
//--------------------------


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


// setup global descriptor table for both
// kernel space & user space.
// local descriptor table is not used.

void 
gdt_init()
{
	set_segment(&gdt[0], 0, 0, 0);
	set_segment(&gdt[1], CODE_SEG_OFFSET, CODE_SEG_LIMIT, CODE_SEG_TYPE);
	set_segment(&gdt[2], DATA_SEG_OFFSET, DATA_SEG_LIMIT, DATA_SEG_TYPE);
	set_segment(&gdt[3], VRAM_SEG_OFFSET, VRAM_SEG_LIMIT, VRAM_SEG_TYPE);
	set_segment(&gdt[4], (uint) (&(cpu.ts)), TSS_SEG_LIMIT, TSS_SEG_TYPE);
	set_segment(&gdt[5], UCODE_SEG_OFFSET, UCODE_SEG_LIMIT, UCODE_SEG_TYPE);
	set_segment(&gdt[6], UDATA_SEG_OFFSET, UDATA_SEG_LIMIT, UDATA_SEG_TYPE);
}


//--------------------------
//
//     function : init
//
//--------------------------


void
kvm_init() 
{
	kvm_setup(kpgtab);
}


//--------------------------
//
//     function : init
//
//--------------------------

// setup user space for initcode.S

void
uvm_setup(struct pte *pgtab, uchar *init, uint sz)
{
	if (sz >= PAGE_SIZE)
		panic("uvm_setup");

	uint pa = page_alloc();
	uchar *mem = (uchar *) pa;
	memset(mem, 0, PAGE_SIZE);
	memmove(mem, init, sz);
	pgtab[KVM_PAGES].pte_p = 1;
	pgtab[KVM_PAGES].pte_w = 1;
	pgtab[KVM_PAGES].pte_u = 1;
	pgtab[KVM_PAGES].pte_ps = 1;
	pgtab[KVM_PAGES].pte_ad = pa >> 12;
}


//--------------------------
//
//  function : critical
//
//--------------------------

// setup kernel virtual memory for
// a given page table.
// when calling kvm_setup 'pgtab' should be locked.

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


//--------------------------
//
//     function : init
//
//--------------------------

// page allocator which allocates physical
// pages started from 4MiB to PHYSTOP
// attach next free page's pa to the current
// free page.
// first phy page is resersed for kernel itself.

void
palloc_init()
{
	lock_init(plist.lock);	
	for (uint i = 1; i < PHY_PAGES - 1; i++) {
		struct phypage *cur = (struct phypage *) (PAGE_SIZE * i);
		cur->next = (struct phypage *) (PAGE_SIZE * (i+1));
	}
	struct phypage *last = (struct phypage *) (PAGE_SIZE * (PHY_PAGES - 1));
	last->next = NULL;
	plist.head = (struct phypage *) (PAGE_SIZE);
}


//--------------------------
//
//   function : critical 
//
//--------------------------

// free a phypage at given pa.
// havoc if a free page is freed.

void
page_free(uint pa)
{
	acquire(plist.lock);
	if (pa % PAGE_SIZE != 0 || (pa / PAGE_SIZE) >= PHY_PAGES)
		panic("page_free: not valid");
	if (pa == 0)
		panic("page_free: kernel space");

	// wipe out all the content
	memset((void *)pa, 0, PAGE_SIZE);

	// add to free_page_list
	struct phypage *p = (struct phypage *) pa;
	p->next = plist.head;
	plist.head = p;
	release(plist.lock);
}


//--------------------------
//
//   function : critical 
//
//--------------------------

// alloc a phypage and return its pa.
// run out of pages if it returns 0.

uint
page_alloc(void)
{	
	acquire(plist.lock);
	if (plist.head == NULL)
		panic("page_alloc: out of pages");
	uint pa = (uint) plist.head;
	plist.head = plist.head->next;
	release(plist.lock);
	return pa;
}

