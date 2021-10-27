#ifndef _VM_H
#define _VM_H

#define GDT_SIZE 7

// segment descriptor details
// offset
#define CODE_SEG_OFFSET		0
#define DATA_SEG_OFFSET		0
#define VRAM_SEG_OFFSET		0xb8000	
#define UCODE_SEG_OFFSET	(1<<30)	// 1GiB
#define UDATA_SEG_OFFSET	(1<<30)	// 1GiB
// limit
#define CODE_SEG_LIMIT	((1<<30)/(1<<12)-1) // 1GiB/4KiB - 1
#define DATA_SEG_LIMIT	((1<<30)/(1<<12)-1)	// 1GiB/4KiB - 1
#define VRAM_SEG_LIMIT	(4-1) // 16KiB/4KiB - 1
#define TSS_SEG_LIMIT		0	// (< 4KiB) 4KiB/4KiB - 1
#define UCODE_SEG_LIMIT	((3<<30)/(1<<12)-1)	// 3GiB/4KiB - 1
#define UDATA_SEG_LIMIT ((3<<30)/(1<<12)-1)	// 3GiB/4KiB - 1
// type
#define CODE_SEG_TYPE		0xC9A	// DPL 0
#define DATA_SEG_TYPE		0xC92	// DPL 0
#define VRAM_SEG_TYPE		0xC92	// DPL 0
#define TSS_SEG_TYPE		0x889	// DPL 0
#define UCODE_SEG_TYPE	0xCFA	// DPL 3
#define UDATA_SEG_TYPE	0xCF2	// DPL 3

// kernel space selectors
#define NULL_SEL	0x00
#define CODE_SEL	0x08	// DPL 0
#define DATA_SEL	0x10	// DPL 0
#define VRAM_SEL	0x18	// DPL 0
#define TSS_SEL		0x20	// DPL 0
#define UCODE_SEL	0x2B	// DPL 3
#define UDATA_SEL	0x33	// DPL 3

// control registers
#define CR0_PE	(1<<0) 
#define CR0_WP	(1<<16)
#define CR0_PG	(1<<31)
#define CR4_PSE	(1<<4)

// eflags 
#define FL_IF	0x00000200

// page size
#define NORM_PAGE_SIZE (1<<12) // for align only
#define PAGE_SIZE (1<<22) // 4 MiB

#define PTE_SIZE	4	// PTE takes 4B
#define PTES	1024	// each pgtab has 1024 PTEs

#define PHY_PAGES	32	// PHY_PAGES <= KVM_PAGES
#define KVM_PAGES	256	// 0-1GiB
#define UVM_PAGES	768	// 1-4GiB

#ifndef __ASSEMBLER__
struct seg_desc 
{
	uint limit_15_0 : 16;
	uint off_15_0 : 16;
	uint off_23_16 : 8;
	uint type_7_0 : 8;
	uint limit_19_16 : 4;
	uint type_11_8 : 4; 
	uint off_31_24 : 8;
};

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

// Task state segment format
struct tss 
{
  uint link;         // Old ts selector
  uint esp0;         // Stack pointers and segment selectors
  ushort ss0;        // after an increase in privilege level
  ushort padding1;
  uint *esp1;
  ushort ss1;
  ushort padding2;
  uint *esp2;
  ushort ss2;
  ushort padding3;
  void *cr3;         // Page directory base
  uint *eip;         // Saved state from last task switch
  uint eflags;
  uint eax;          // More saved state (registers)
  uint ecx;
  uint edx;
  uint ebx;
  uint *esp;
  uint *ebp;
  uint esi;
  uint edi;
  ushort es;         // Even more saved state (segment selectors)
  ushort padding4;
  ushort cs;
  ushort padding5;
  ushort ss;
  ushort padding6;
  ushort ds;
  ushort padding7;
  ushort fs;
  ushort padding8;
  ushort gs;
  ushort padding9;
  ushort ldt;
  ushort padding10;
  ushort t;          // Trap on task switch
  ushort iomb;       // I/O map base address
};
#endif

#endif
