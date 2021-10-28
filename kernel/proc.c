// All APs are guaranteed to start only after all *init* are done!
// And IF flags are not set until the first entering to user space.
// Tag *shared* denotes global variables that might be contented by
// different threads both in unicode or multicore environments.
// Tag *read only* denotes global variables that shall not be modified
// and they have already been initialized by functions with Tag *init*.
// Tag *critical* denotes functions containing at lease one critical section.
// Within those functions the proper use of locks or alternatives is required.


#include "defs.h"


//--------------------------
//
//    global variables
//
//--------------------------


/* shared */

struct {
	lock *lock;
	struct proc procs[PROCS];
} ptable;


/* shared */

struct cpu cpu;


//--------------------------
//
//    functions : init
//
//--------------------------


void 
proc_init()
{
	lock_init(ptable.lock);
	for (uint i = 0; i < PROCS; i++) {
		ptable.procs[i].state = UNUSED;
		ptable.procs[i].pid = i;
	}
}


//--------------------------
//
//    functions : init
//
//--------------------------


static inline void
flush_segr()
{
	asm volatile(
		"mov %0, %%eax\n\t"
		"mov %%eax, %%ds\n\t"
		"mov %0, %%eax\n\t"
		"mov %%eax, %%es\n\t"
		"mov %0, %%eax\n\t"
		"mov %%eax, %%ss\n\t"
		"mov %0, %%eax\n\t"
		"mov %%eax, %%fs\n\t"
		"mov %0, %%eax\n\t"
		"mov %%eax, %%gs\n\t"
		:: "i" (DATA_SEL));
}


static void
enable_paging()
{
	asm volatile (	
		"movl %%cr4, %%eax\n\t"	// turn on page size extension
		"orl %1, %%eax\n\t"
		"movl %%eax, %%cr4\n\t"
		"movl %0, %%cr3\n\t"	// set page directory
		"movl %%cr0, %%eax\n\t"	// turn on paging
		"orl %2, %%eax\n\t"
		"movl %%eax, %%cr0\n\t"
		:: "r" (kpgtab), "i" (CR4_PSE), "i" (CR0_PG|CR0_WP)
		: "eax" );	
}


void 
cpu_init()
{
	lgdt((uint) gdt, sizeof(gdt));	// laod system regs
	lidt((uint) idt, sizeof(idt));
	ltr(TSS_SEL);
	flush_segr();	// flush segment regs
	enable_paging();	// enable paging
	cpu.proc = NULL;	// prepare for proc to load in
	cpu.loaded = false;
}


//--------------------------
//
//   functions : critical 
//
//--------------------------


static void
forkret()
{
	release(ptable.lock);	
}

struct proc *
proc_alloc()
{
	struct proc *p = NULL;
	uchar *sp;

	acquire(ptable.lock);
	for (uint i = 0; i < PROCS; i++) {
		if (ptable.procs[i].state == UNUSED) {
			p = &(ptable.procs[i]);
			break;
		}
	}
	if (!p) {
		release(ptable.lock);
		return NULL;	// no proc available
	}
	p->state = EMBRYO;	// lock p
	release(ptable.lock);

	sp = p->kstack + KSTACK_SIZE;	// initialize kstack
	kvm_setup(p->pgtab);	// initialize pgtab
	sp -= sizeof(*(p->tf));	// initialize trapframe
	p->tf = (struct trapframe *) sp;
	sp -= 4;	
	*(uint *)sp = (uint) trapret;
	sp -= sizeof(*(p->ctx));	// initialize context
	p->ctx = (struct context *) sp;
	p->ctx->eip = (uint) forkret;
	
	return p;
}


//--------------------------
//
//    functions : init
//
//--------------------------


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


//--------------------------
//
//   functions : critical 
//
//--------------------------


static void
proc_load(struct proc *p)
{
	cpu.proc = p;
	cpu.loaded = true;
	cpu.ts.ss0 = DATA_SEL;
	cpu.ts.esp0 = (uint) (p->kstack + KSTACK_SIZE);
	lcr3((uint) p->pgtab);
	cpu.proc->state = RUNNING;
}


static void 
proc_unload()
{
	lcr3((uint) kpgtab);
	cpu.loaded = false;
	cpu.proc = NULL;
}


void 
scheduler() 
{
	for (;;) {
		acquire(ptable.lock);
		for (int i = 0; i < PROCS; i++) {
			if (ptable.procs[i].state != RUNNABLE) {
				continue;
			}	
			proc_load(&ptable.procs[i]);
			swtch(&(cpu.sched_ctx), cpu.proc->ctx);
			cprintln("swtich", TYPE_STR);
			proc_unload();
		}
		release(ptable.lock);
	}
}


//--------------------------
//
//   functions : critical 
//
//--------------------------


// only three functions will trigger swtch
// namely yield(), sleep() & exit().

void
yield()
{
	acquire(ptable.lock);
	cpu.proc->state = RUNNABLE;
	swtch(&(cpu.proc->ctx), cpu.sched_ctx);
	release(ptable.lock);
}


void
sleep() 
{

}


void 
exit()
{

}


//--------------------------
//
//   functions : critical 
//
//--------------------------


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
wait()
{

}

/* critical */
void
wakeup() 
{

}

/* critical */
void 
kill()
{

}
