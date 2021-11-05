#ifndef _FS_H
#define _FS_H

//  +-------------+-----------------+----------------+-----------------+----------------------+
//  | B0 : Unused | B1 : Superblock | B2-BX : Inodes | BX-BY : Bit Map |	BY-BZ : Data Blocks |
//  +-------------+-----------------+----------------+-----------------+----------------------+

#define ROOTDEV 				1
#define BLOCK_SIZE			512
#define INODES					12												// in-memory inodes bufs

#define IPB							(BLOCK_SIZE/sizeof(struct dinode))
#define BPB							(BLOCK_SIZE*8)
#define BOOTBLOCK				0													// bootblock offset
#define SUPERBLOCK			1													// superblock offset
#define INODEBLOCK(x)		((x/IPB)+SUPERBLOCK+1)		// inode block offset, x = 0 denotes the first inode entry.

#define DIRECTS					12												// directly indexed blocks
#define INDIRECTS				(BLOCK_SIZE/sizeof(uint))	// indirectly indexes blocks
#define FILE_BLOCKS			(DIRECTS+1)
#define MAX_FILE				(DIRECTS+INDIRECTS)

#define DIRENTS					14												// max entries in one directory.

#define FILES						32												// max files in ftable

typedef enum {
	EMPTY,
	FILE,
	DIR,
	DEV
} inode_t;

struct superblock {
	uint n_block;				// how many blocks
	uint n_inode_block;	// how many inode blocks
	uint n_bmap_block;	// how many bit map blocks
	uint n_data_block;	// how many data blocks
};

struct dinode {
	inode_t type;
	uint major;		// for type = DEV
	uint minor;		// for type = DEV
	uint n_link;	// how many dirents referring to it
	uint size;		// size of file (bytes)
	uint ads[FILE_BLOCKS];
};

struct inode {
	uint dev;		// inode(dev, inum) 
	uint inum;
	uint ref;		// free if ref = 0
	bool lock;	
	struct dinode data;
};

struct file {
	enum { FD_NONE, FD_PIPE, FD_INODE } type;
	int ref;
	bool readable;
	bool writable;
//	struct pipe *pipe;
	struct inode *ip;
	uint off;
};

#endif
