#include "defs.h"

#define KSTACK_SIZE 4096

/* shared */
struct proc_list plist;

/* shared */
// page tables for procs
__attribute__((__aligned__(NORM_PAGE_SIZE)))
struct pte pgtabs[PROCS][PTES];

/* shared */
// kernel stacks for procs
uchar kstacks[PROCS][KSTACK_SIZE];

/* init */
void 
proc_init()
{
	for (uint i = 0; i < PROCS; i++) {
		plist.procs[i].state = UNUSED;
		plist.procs[i].pid = i;
	}
}

/* critical */
struct proc *
proc_alloc()
{
	struct proc *p = NULL;
	uchar *esp;

	for (uint i = 0; i < PROCS; i++) {
		if (plist.procs[i].state == UNUSED) {
			p = &(plist.procs[i]);
			break;
		}
	}

	// if not found
	if (!p) 
		return NULL;

	// update state
	p->state = EMBRYO;

	// initialize kstack
	p->kstack = kstacks[p->pid] + KSTACK_SIZE;
	esp = p->kstack;

	// initialize pgtab
	p->pgtab = pgtabs[p->pid];	
	kvm_setup(p->pgtab);

	// initialize trapframe
	esp -= sizeof(*(p->tf));
	p->tf = (struct trapframe *) esp;

	// initialize context
	esp -= sizeof(*(p->ctx));
	p->ctx = esp;

	return p;
}

/* init */
// setup first proc: init
void 
user_init()
{
	
}

/* critical */
void
fork()
{

}

/* critical */
void
exec()
{

}

/* critical */
void 
exit()
{

}

/* critical */
void
wait()
{

}

/* critical */
void
yield()
{

}

/* critical */
void
sleep() 
{

}

/* critical */
void
wakeup() 
{

}

/* critical */
void 
scheduler() 
{

}

/* critical */
void
sched()
{

}

/* critical */
void 
kill()
{

}

/* critical */
void
swich()
{

}
