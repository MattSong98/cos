#include "defs.h"


//--------------------------
//
//    global variables
//
//--------------------------


/* shared */

struct {
	struct proc procs[PROCS];
} proc_table;


/* shared */

struct cpu cpu;


//--------------------------
//
//        functions
//
//--------------------------


/* init */

void 
proc_init()
{
	ltr(TSS_SEL);
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

	p->state = EMBRYO;	// update state
	sp = p->kstack + KSTACK_SIZE;	// initialize kstack
	kvm_setup(p->pgtab);	// initialize pgtab
	sp -= sizeof(*(p->tf));	// initialize trapframe
	p->tf = (struct trapframe *) sp;
	sp -= sizeof(*(p->ctx));	// initialize context
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
	p->tf->eflags = FL_IF;
	p->tf->esp = PAGE_SIZE;
	p->tf->eip = 0;

	p->state = RUNNABLE;
}


static void
proc_load(struct proc *p)
{
	tss_setup(DATA_SEL, (uint) (p->kstack + KSTACK_SIZE));
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
	cpu.loaded = false;

	for (;;) {
		for (int i = 0; i < PROCS; i++) {
			if (proc_table.procs[i].state != RUNNABLE) {
				continue;
			}	
		
			cpu.loaded = true;	
			cpu.proc = &proc_table.procs[i];
			proc_load(cpu.proc);
			cpu.proc->state = RUNNING;
			swtch(&(cpu.sched_ctx), cpu.proc->ctx);
			cpu.proc->state = RUNNABLE;
			proc_unload();
			cpu.proc = NULL;
			cpu.loaded = false;
		}
	}
}


/* critical */

void
yield()
{
	swtch(&(cpu.proc->ctx), cpu.sched_ctx);
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
