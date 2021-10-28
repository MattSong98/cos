#include "defs.h"

void 
lock_init(lock *lock) 
{
	*lock = unlocked;	
}

void 
acquire(lock *lock)
{
	while (xchg(lock, locked) == locked);
}

void 
release(lock *lock) 
{
	if (xchg(lock, unlocked) == unlocked)
		panic("release");
}
