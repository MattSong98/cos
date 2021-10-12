#ifndef _DEFS_H
#define _DEFS_H

// pic.c

extern void pic_init(void);
extern void pic_enable_irq(uchar);
extern void pic_send_eoi(uchar);

// console.c

extern void init_cga(void);
extern void write_cga(void *ptr, int type);
extern void clear_cga(void);

// kbd.c
extern void kbd_init(void);
extern int kbdgetc(void);

// mm.c

extern void init_memory(void);
extern uint get_free_page(void);
extern int free_page(uint);

// panic.c

extern void panic(void);

#endif
