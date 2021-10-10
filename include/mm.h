#ifndef _MM_H
#define _MM_H

#define PAGE_SIZE 4096

extern void init_memory();
extern unsigned long get_free_page();
extern int free_page(unsigned long);

#endif
