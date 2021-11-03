#ifndef _EXT2_H
#define _EXT2_H

// block 0 : unused
// block 1 : superblock
// block 2 - ? : inode table
// block : data blocks

#define BLOCK_SIZE	512
#define DIRECTS			12 // max directly indexed blocks a file can possess.
#define INDIRECTS		(BLOCK_SIZE / sizeof(uint))	// indirectly indexes blocks.

enum file_t {
	FILE,
	DIR,
	DEV
};

struct superblock {
	uint n_block;	// how many blocks
	uint n_inode;	// how many inodes
	uint n_data_block;	// how many data blocks
	uint n_log_block;	// how many log blocks
};

struct dinode {
	file_t type;
	ushort major;
	ushort minor;
	ushort n_link;	// how many dirent referring to it
	uint size;	// size of file (bytes)
	uint ads[NDIRECT+1];
};

struct inode {
	uint dev;
	uint inum;
	int ref;
	int flags;

	// copy from dinode
	file_t type;
	ushort major;
	ushort minor;
	ushort n_link;
	uint size;
	uint ads[NDIRECT+1];
};

struct file {
	enum { FD_NONE, FD_PIPE, FD_INODE } type;
	int ref;
	bool readable;
	bool writable;
//	struct pipe *pipe;
	struct inode *ip;
	uint off;
}

#endif
