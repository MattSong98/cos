#ifndef _MM_H
#define _MM_H

#include "types.h"

#define PAGE_SIZE 4096

extern void init_memory();
extern uint get_free_page();
extern int free_page(uint);

#endif
