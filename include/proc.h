#ifndef _PROC_H
#define _PROC_H

#define PROCS 16
#define PROC_NAME_SIZE	16
#define KSTACK_SIZE	4096

enum proc_state 
{ 
	UNUSED, 
	EMBRYO, 
	SLEEPING, 
	RUNNABLE, 
	RUNNING, 
	ZOMBIE 
};

struct context 
{
	uint edi;
	uint esi;
	uint ebx;
	uint ebp;
	uint eip;
};

struct proc 
{
	// uint sz;
	__attribute__((__aligned__(NORM_PAGE_SIZE)))
	struct pte pgtab[PTES];
	uchar kstack[KSTACK_SIZE];
	enum proc_state state;
	uint pid;
	struct proc *parent;
	struct trapframe *tf;	// refer to the first trapframe (U => K)
	struct context *ctx;
	void *channel;	// resource to wait for
	// int killed;
	// struct file *ofile[NOFILE];
	struct inode *cwd;
	char name[PROC_NAME_SIZE];
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

struct cpu {
	struct tss ts;
	struct context *sched_ctx;
	struct proc *proc;
};

#endif
