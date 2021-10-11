#ifndef _DEFS_H
#define _DEFS_H

#include "types.h"

// console.c

#define TYPE_HEX 0
#define TYPE_STR 1
#define TYPE_CHAR 2

extern void init_cga();
extern void write_cga(void *ptr, int type);
extern void clear_cga();

// kbd.c
extern void init_kbd();
extern int kbdgetc();

// mm.c

extern void init_memory();
extern uint get_free_page();
extern int free_page(uint);

// panic.c

extern void panic();

#endif
