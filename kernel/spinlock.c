#include "defs.h"

void 
lock_init(lock *l) 
{
	*l = unlocked;	
}

void 
acquire(lock *l)
{
	while (xchg(l, locked) == locked); 
}

void 
release(lock *l) 
{
	if (xchg(l, unlocked) == unlocked)
		panic("release");
}
