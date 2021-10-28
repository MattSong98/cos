#ifndef _TYPES_H
#define _TYPES_H

// Attension! types.h is almost used by every header
// file therefore it shall be the head of 
// the including queue.

#define NULL 0

typedef enum {
	unlocked,
	locked
} lock;

typedef enum {
	false,
	true
} bool;

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#endif
