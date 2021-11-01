#include "defs.h"

void 
lock_init(lock *lock) 
{
	*lock = unlocked;	
}

void 
acquire(lock *lock)
{
	// lprintf("acquiring", TYPE_STR);
	while (xchg(lock, locked) == locked);
}

void 
release(lock *lock) 
{
	// lprintf("releasing", TYPE_STR);
	if (xchg(lock, unlocked) == unlocked)
		panic("release");
}
