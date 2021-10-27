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

/* shared */
struct context *scheduler_context;

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

	extern uchar _binary_target_initcode_start[];
	extern uchar _binary_target_initcode_size[];
	
	// initialize the trivial
	strcpy(p->name, "init");	
	p->parent = NULL;

	// initialize user space
	uvm_setup(p->pgtab, _binary_target_initcode_start, (uint)_binary_target_initcode_size);

	// setup trapframe
	memset(p->tf, 0, sizeof(*(p->tf)));
	p->tf->cs = UCODE_SEL;
	p->tf->ds = UDATA_SEL;
	p->tf->es =	UDATA_SEL;
	p->tf->ss = UDATA_SEL;
	p->tf->fs = UDATA_SEL;
	p->tf->gs = UDATA_SEL;
	// p->tf->eflags = FL_IF;
	p->tf->esp = PAGE_SIZE;
	p->tf->eip = 0;

	p->state = RUNNABLE;
}

static void
proc_load(struct proc *p)
{
	tss_setup(DATA_SEL, (uint) p->kstack);
	ltr(TSS_SEL);
	lcr3((uint) p->pgtab);
}

static void 
proc_unload()
{
	lcr3((uint) kpgtab);
}

/* critical */
void 
scheduler() 
{	
	struct proc *p;
	for (;;) {
		// sti();

		for (int i = 0; i < PROCS; i++) {
			if (proc_table.procs[i].state != RUNNABLE) 
				continue;
			p = &proc_table.procs[i];
			proc_load(p);
			p->state = RUNNING;
			// here it goes wrong	
			swtch(&scheduler_context, p->ctx);
			proc_unload();
		}
	}
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
sched()
{

}

/* critical */
void 
kill()
{

}
