#define TYPE_HEX 0
#define TYPE_STR 1

extern void init_cga();
extern void write_cga(void *ptr, int type);
extern void clear_cga();
