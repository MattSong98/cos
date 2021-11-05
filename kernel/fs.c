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


static struct superblock
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


// alloc a free data block and return its block_loc.
// panic if it runs out of block.
// the upper boundary of available data blocks is determined
// by min(n_data_block, BPB * n_bmap_block).


uint 
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
				if (block_bias >= sb.n_data_block) 
					panic("balloc: out of block");
				p->data[j] |= (1 << k);
				ide_brelse(p);
				return (bmap_off + sb.n_bmap_block) + block_bias;
			}
		}
		ide_brelse(p);
	}
	panic("balloc: out of block");
	return 0;
}


// free a block at given block_loc in dev.
// erase all the content.
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
	// erase content
	struct ide_buf *q = ide_bget(dev, block_loc, IDE_RW);
	memset(q->data, 0, sizeof(q->data));
	ide_brelse(q);
	ide_brelse(p);
}


//--------------------------
//
//   function : critical
//
//--------------------------


uint
inode_alloc(uint dev, inode_t type)
{
	struct superblock sb = superblock_load();
	uint inode_off = 2;
	for (uint i = inode_off; i < inode_off + sb.n_inode_block; i++) {
		struct ide_buf *p = ide_bget(dev, i, IDE_RW);
		for (uint j = 0; j < IPB; j++) {
			struct dinode *dip = (struct dinode *) &p->data[j * sizeof(struct dinode)];
			if (dip->type != EMPTY)
				continue;
			// found
			dip->type = type;
			dip->major = 0;
			dip->minor = 0;
			dip->n_link = 0;
			dip->size = 0;
			memset(dip->ads, 0, sizeof(dip->ads));
			ide_brelse(p);
			return (i - inode_off) * IPB + j;
		}
		ide_brelse(p);
	}
	panic("inode_alloc: run out of dinode");
	return 0;
}


// sync dinode (in-disk inode) with inode (in-memory inode)
// caller must hold the lock before calling inode_sync().


static void 
inode_sync(struct inode *ip) 
{
	struct ide_buf *p = ide_bget(ip->dev, INODEBLOCK(ip->inum), IDE_RW);
	struct dinode *dip = (struct dinode *) &p->data[(ip->inum % IPB) * sizeof(struct dinode)];
	dip->type = ip->data.type;
	dip->major = ip->data.major;
	dip->minor = ip->data.minor;
	dip->n_link = ip->data.n_link;
	dip->size = ip->data.size;
	memmove(dip->ads, ip->data.ads, sizeof(dip->ads));
	ide_brelse(p);
}


// free the given inode 
// oposite of inode_alloc
// only called by inode_put()


static void
inode_trunc(struct inode *ip)
{
	for (uint i = 0; i < DIRECTS; i++) {
		if (ip->data.ads[i] != 0) 
			bfree(ip->dev, ip->data.ads[i]);
	}
	
	uint indir = ip->data.ads[DIRECTS];
	if (indir != 0) {
		struct ide_buf *p = ide_bget(ip->dev, indir, IDE_RO);
		for (uint i = 0; i < INDIRECTS; i++) {
			uint ad = *((uint *) p->data + i);
			if (ad != 0)
				bfree(ip->dev, ad);
		}
		ide_brelse(p);
		bfree(ip->dev, indir);
	}

	ip->data.type = EMPTY;
	ip->data.major = 0;
	ip->data.minor = 0;
	ip->data.n_link = 0;
	ip->data.size = 0;
	memset(ip->data.ads, 0, sizeof(ip->data.ads));
	inode_sync(ip);
}


// get a reference to inode(dev, inum)
// alloc a new inode(dev, inum) from inode_cache if not exist.


struct inode *
inode_get(uint dev, uint inum)
{
	struct inode *empty = NULL;
	acquire(&inode_cache.lock);
	for (uint i = 0; i < INODES; i++) {
		if (inode_cache.inodes[i].ref > 0) {
			if (inode_cache.inodes[i].dev == dev && inode_cache.inodes[i].inum == inum) {
				inode_cache.inodes[i].ref++;
				release(&inode_cache.lock);
				return &inode_cache.inodes[i];
			}
		} else if (empty == NULL && inode_cache.inodes[i].ref == 0) {
			empty = &inode_cache.inodes[i];
		}
	}

	// alloc a new inode
	if (empty == NULL) {
		panic("inode_get: run out of inodes");
	}
	empty->dev = dev;
	empty->inum = inum;
	empty->ref = 1;
	empty->lock = false;
	struct ide_buf *p = ide_bget(dev, INODEBLOCK(inum), IDE_RO);
	struct dinode *dip = (struct dinode *) &p->data[(inum % IPB) * sizeof(struct dinode)];
	empty->data.type = dip->type;
	empty->data.major = dip->major;
	empty->data.minor = dip->minor;
	empty->data.n_link = dip->n_link;
	empty->data.size = dip->size;
	memmove(empty->data.ads, dip->ads, sizeof(dip->ads));
	ide_brelse(p);
	release(&inode_cache.lock);
	
	if (empty->data.type == EMPTY)
		panic("inode_get: type empty");
	return empty;
}


// return 'ref' back


void
inode_put(struct inode *ip)
{
	acquire(&inode_cache.lock);
	if (ip->lock == true)
		panic("inode_put: inode locked");
	if (ip->ref == 0) {
		panic("inode_put: no ref");
	}
	if (--ip->ref == 0 && ip->data.n_link == 0)
		inode_trunc(ip);
	release(&inode_cache.lock);
}


// caller holding a reference of inode ip calls inode_lock
// to lock it up for exclusive use.
// it's inode_lock that syncs inode with dinode.


void
inode_lock(struct inode *ip) 
{
	acquire(&inode_cache.lock);
	if (ip->ref == 0)
		panic("inode_lock: no ref");

	while (ip->lock == true) {
		sleep(ip, &inode_cache.lock);
	}
	ip->lock = true;
	release(&inode_cache.lock);
}


void 
inode_unlock(struct inode *ip)
{
	acquire(&inode_cache.lock);
	if (ip->lock == false)
		panic("inode_unlock: inode not locked");
	ip->lock = false;
	wakeup(ip);
	release(&inode_cache.lock);
}


// return the address of (n+1)th data block of inode p
// alloc a new one if it doesn't exist.
// return -1 if run out of data block


static uint
bmap(struct inode *ip, uint n)
{
	if (n < DIRECTS) {
		if (ip->data.ads[n] == 0) {
			ip->data.ads[n] = balloc(ip->dev);
			inode_sync(ip);
		}
		return ip->data.ads[n];	
	}

	n -= DIRECTS;
	if (n >= INDIRECTS)
		panic("bmap: out of range");

	if (ip->data.ads[DIRECTS] == 0) {
		ip->data.ads[DIRECTS] = balloc(ip->dev);
		inode_sync(ip);
		struct ide_buf *p = ide_bget(ip->dev, ip->data.ads[DIRECTS], IDE_RW);
		memset(p->data, 0, sizeof(ip->data));
		ide_brelse(p);
	}
		
	struct ide_buf *p = ide_bget(ip->dev, ip->data.ads[DIRECTS], IDE_RW);
	uint *ad = (uint *) p->data + n;
	if (*ad == 0) 
		*ad = balloc(ip->dev);
	ide_brelse(p);
	return *ad;
}


int 
inode_read(struct inode *ip, char *dst, uint off, uint n)
{
	if (off > ip->data.size)
		return -1;
	if (off + n > ip->data.size)
		n = ip->data.size - off;

	for (uint tot = 0, m = 0; tot < n; tot += m, off += m, dst += m) {
		struct ide_buf *p = ide_bget(ip->dev, bmap(ip, off / BLOCK_SIZE), IDE_RO);
		m = (n - tot) < (BLOCK_SIZE - off % BLOCK_SIZE) ? (n - tot) : (BLOCK_SIZE - off % BLOCK_SIZE);
		memmove(dst, &p->data[off % BLOCK_SIZE], m);
		ide_brelse(p);
	}
	return n;
}


int
inode_write(struct inode *ip, char *src, uint off, uint n)
{
	if (off > ip->data.size)
		return -1;
	if (off + n > BLOCK_SIZE * MAX_FILE)
		return -1;

	for (uint tot = 0, m = 0; tot < n; tot += m, off += m, src += m) {
		struct ide_buf *p = ide_bget(ip->dev, bmap(ip, off / BLOCK_SIZE), IDE_RW);
		m = (n - tot) < (BLOCK_SIZE - off % BLOCK_SIZE) ? (n - tot) : (BLOCK_SIZE - off % BLOCK_SIZE);
		memmove(&p->data[off % BLOCK_SIZE], src, m);
		ide_brelse(p);
	}

	if (n > 0 && off > ip->data.size) {
		ip->data.size = off;
		inode_sync(ip);
	}
	return n;
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


