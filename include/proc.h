#ifndef _PROC_H
#define _PROC_H

#define PROCS 32
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
	struct pte *pgtab;
	uchar *kstack;
	enum proc_state state;
	int pid;
	struct proc *parent;
	struct trapframe *tf;
	struct context *ctx;
	// void *chan;
	// int killed;
	// struct file *ofile[NOFILE];
	// struct inode *cwd;
	char name[PROC_NAME_SIZE];
};

struct proc_list {
	struct proc procs[PROCS];
};

#endif
