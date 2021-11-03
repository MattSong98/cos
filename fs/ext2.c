#include "defs.h"

#define INODES	64	// in-memory inodes bufs

//--------------------------
//
//    global variables
//
//--------------------------


/* shared */

struct {
	lock lock;
	struct inode inodes[INODES];
} inode_cache;


/* shared */

struct {
	lock lock;
	struct file file[FILES];
} ftable;


//--------------------------
//
//   function : init 
//
//--------------------------


void 
inode_init()
{
	lock_init(&inode_cache.lock);
}


//--------------------------
//
//   function : init 
//
//--------------------------


void 
file_init()
{
	lock_init(&ftable.lock);
}


//--------------------------
//
//   function : critical
//
//--------------------------

void
balloc()
{

}

void 
bfree()
{

}

//--------------------------
//
//   function : critical
//
//--------------------------


struct inode *
inode_alloc(uint dev, type_t type)
{
	

}


static void
inode_trunc(struct inode *p)
{


}

// sync dinode (in-disk inode) with inode (in-memory inode)

void 
inode_sync(struct inode *p) 
{
	
}

static struct inode *
inode_get(uint dev, uint inum)
{


}

static void
inode_put(struct inode *p)
{


}

void
inode_lock(struct inode *p) 
{

}

void 
inode_unlock(struct inode *p)
{

}


//--------------------------
//
//   function : critical
//
//--------------------------


// return the address of nth data block of inode p
// alloc a new one if it doesn't exist.

static uint
bmap(struct inode *p, uint n)
{


}


// discard contents of inode p.
// only called after the last dirent referring
// to this inode has been erased on disk.

int 
readi(struct inode *p, char *dst, uint off, uint n)
{

}

int
write(struct inode *p, char *src, uint off, uint n)
{

}

//--------------------------
//
//   function : critical
//
//--------------------------


// look for a directory entry in a directory.
// if found set *poff to byte offset of entry.
// caller must have already locked up.

struct inode *
dir_lookup(struct inode *dp, char *name, uint *poff) 
{

}


// write a new directory entry (name, inum) into the directory dp.


int 
dirlink(struct inode *dp, char *name, uint inum)
{


}


//--------------------------
//
//   function : critical
//
//--------------------------


