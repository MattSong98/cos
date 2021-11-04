#include "defs.h"


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
fs_init()
{
	lock_init(&inode_cache.lock);
	lock_init(&ftable.lock);
}


//--------------------------
//
//   function : critical
//
//--------------------------


struct superblock
superblock_load()
{
	struct superblock sb;
	struct ide_buf *p = ide_bget(ROOTDEV, SUPERBLOCK, IDE_RO);
	memmove(&sb, p->data, sizeof(struct superblock));
	ide_brelse(p);
	return sb;
}


void
superblock_store(struct superblock sb)
{
	struct ide_buf *p = ide_bget(ROOTDEV, SUPERBLOCK, IDE_RW);
	memmove(p->data, &sb, sizeof(struct superblock));
	ide_brelse(p);
}


// alloc a free data block and return its block offset.
// if not found return -1.
// the upper boundary of available data blocks is determined
// by min(n_data_block, BPB * n_bmap_block).

int
balloc(uint dev)
{
	struct superblock sb = superblock_load();
	uint bmap_off = sb.n_inode_block + 2;
	for (uint i = bmap_off; i < bmap_off + sb.n_bmap_block; i++) {
		struct ide_buf *p = ide_bget(dev, i, IDE_RW);
		// char *data = p->data;
		for (uint j = 0; j < IDE_BUF_SIZE; j++) {
			if (p->data[j] == 0xFF)
				continue;
			for (uint k = 0; k < 8; k++) {
				if ((p->data[j] | (1 << k)) == p->data[j])
					continue;
				// found
				uint block_bias = (i - bmap_off) * BPB + j * 8 + k;	// start from 0
				if (block_bias >= sb.n_data_block) {
					ide_brelse(p);
					return -1;
				}
				p->data[j] |= (1 << k);
				ide_brelse(p);
				return (bmap_off + sb.n_bmap_block) + block_bias;
			}
		}
		ide_brelse(p);
	}
	return -1;
}


// free a block at given block_off in dev.
// panic if it's out of range. 
// panic if it's been already freed.
// block_loc = block_off + block_bias

void 
bfree(uint dev, uint block_loc)
{
	struct superblock sb = superblock_load();
	uint block_off = sb.n_bmap_block + sb.n_inode_block + 2;
	if (block_loc < block_off) 
		panic("bfree: out of range");
	if (block_loc >= block_off + sb.n_data_block)
		panic("bfree: out of range");
	if (block_loc >= block_off + BPB * sb.n_bmap_block)
		panic("bfree: out of range");

	uint block_bias = block_loc - block_off;
	uint bmap_loc = (block_bias / BPB) + sb.n_inode_block + 2;
	struct ide_buf *p = ide_bget(dev, bmap_loc, IDE_RW);
	if (((p->data[(block_bias % BPB) / 8]) & (1 << (block_bias % 8))) == 0) 
		panic("bfree: block bas been freed");
	p->data[(block_bias % BPB ) / 8] &= ~(1 << (block_bias % 8));
	ide_brelse(p);
}


//--------------------------
//
//   function : critical
//
//--------------------------


struct inode *
inode_alloc(uint dev, inode_t type)
{
	return NULL;
}


void
inode_trunc(struct inode *p)
{


}

// sync dinode (in-disk inode) with inode (in-memory inode)

void 
inode_sync(struct inode *p) 
{
	
}

struct inode *
inode_get(uint dev, uint inum)
{
	return NULL;
}

void
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

uint
bmap(struct inode *p, uint n)
{
	return 0;
}


// discard contents of inode p.
// only called after the last dirent referring
// to this inode has been erased on disk.

int 
readi(struct inode *p, char *dst, uint off, uint n)
{
	return 0;
}

int
write(struct inode *p, char *src, uint off, uint n)
{
	return 0;
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
	return NULL;
}


// write a new directory entry (name, inum) into the directory dp.


int 
dirlink(struct inode *dp, char *name, uint inum)
{
	return 0;
}


//--------------------------
//
//   function : critical
//
//--------------------------


