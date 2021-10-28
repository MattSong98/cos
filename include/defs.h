#ifndef _DEFS_H
#define _DEFS_H

// Every header file included shall be self-dependent to keep 
// away from "undefined errors" generated by the complier.
// In principle, all the headers should be independent,
// so they can be organized in any order within the including
// queue. However, frankly, that's hard to achieve without proper
// using of #include directive inside the header file itself.

#include "types.h"
#include "x86.h"
#include "kbd.h"
#include "ide.h"
#include "console.h"
#include "vm.h"
#include "pic.h"
#include "trap.h"
#include "timer.h"
#include "string.h"
#include "proc.h"

// Global Functions & Variables

// trapasm.S
extern uint vectors[];	// variable
extern void trapret(void);

// swtch.S
extern void swtch(struct context **old, struct context *new);

// main.c
extern int main(void);

// spinlock.c
extern void lock_init(lock *);
extern void acquire(lock *);
extern void release(lock *);

// trap.c
extern void trap_init(void);
extern void trap(struct trapframe *);

// syscall.c
extern void syscall(void);

// pic.c
extern void pic_init(void);
extern void pic_enable_irq(uchar);
extern void pic_send_eoi(uchar);

// console.c
extern void console_init(void);
extern void cprintf(const void *, int);
extern void console_intr(int);
extern void cprintln(const void *, int);

// ide.c
extern void ide_init(void);
extern void ide_intr(void);
extern struct ide_buf *ide_bget(uint, uint, uint);
extern void ide_brelse(struct ide_buf *);

// timer.c
extern void timer_init(void);

// kbd.c
extern void kbd_init(void);
extern int kbdgetc(void);
extern void kbd_intr(void);

// proc.c
extern struct cpu cpu;	// variable
extern void proc_init(void);
extern struct proc *proc_alloc(void);
extern void user_init(void);
extern void fork(void);
extern void exec(void);
extern void exit(void);
extern void wait(void);
extern void yield(void);
extern void sleep(void);
extern void wakeup(void);
extern void scheduler(void);
extern void kill(void);

// vm.c
extern struct pte kpgtab[];	// variable
extern void kvm_init(void);
extern void palloc_init(void);
extern void page_free(uint);
extern uint page_alloc(void);
extern void kvm_setup(struct pte *);
extern void gdt_init(void);
extern void uvm_setup(struct pte *, uchar *, uint);
extern void tss_setup(ushort, uint);

// panic.c
extern void panic(const char *);

// string.c
extern void strcpy(char *, char *);
extern void *memset(void *, int, uint);
extern void *memmove(void *, const void *, uint);

#endif
