#ifndef _CGA_H
#define _CGA_H

#define TYPE_HEX 0
#define TYPE_STR 1
#define TYPE_CHAR 2

extern void init_cga();
extern void write_cga(void *ptr, int type);
extern void clear_cga();

#endif
