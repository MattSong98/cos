#include "defs.h"

/* shared */
struct {
	struct proc procs[PROCS];
} proc_table;

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
		proc_table.procs[i].state = UNUSED;
		proc_table.procs[i].pid = i;
	}
}

/* critical */
struct proc *
proc_alloc()
{
	struct proc *p = NULL;
	uchar *sp;

	for (uint i = 0; i < PROCS; i++) {
		if (proc_table.procs[i].state == UNUSED) {
			p = &(proc_table.procs[i]);
			break;
		}
	}
	if (!p) return NULL;	// no proc available

	// update state
	p->state = EMBRYO;

	// initialize kstack
	p->kstack = kstacks[p->pid] + KSTACK_SIZE;
	sp = p->kstack;

	// initialize pgtab
	p->pgtab = pgtabs[p->pid];	
	kvm_setup(p->pgtab);

	// initialize trapframe
	sp -= sizeof(*(p->tf));
	p->tf = (struct trapframe *) sp;

	// initialize context
	sp -= sizeof(*(p->ctx));
	p->ctx = (struct context *) sp;
	p->ctx->eip = (uint) trapret;
	
	return p;
}

/* init */
// create first proc: init
void 
user_init()
{
	struct proc *p = proc_alloc();
	if (!p) panic("user_init");
	
	// initialize the trivial
	strcpy(p->name, "init");	
	p->parent = NULL;

	// setup trapframe
	p->tf->cs = NULL;
	p->tf->ds = NULL;
	p->tf->es =	NULL;
	p->tf->ss = NULL;
	p->tf->fs = NULL;
	p->tf->gs = NULL;
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
	sti();
	for (;;) {
		;
	}
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
