#ifndef _CONSOLE_H
#define _CONSOLE_H

#define TYPE_HEX 0
#define TYPE_STR 1
#define TYPE_CHAR 2

#define CGA_SIZE 80*25
#define CGA_ROW_SIZE 25
#define CGA_COL_SIZE 80
#define CGA_STD_ATR 0x07

extern void 
_copy_to_cga (uchar c, uchar atr, ushort pos);

#endif
