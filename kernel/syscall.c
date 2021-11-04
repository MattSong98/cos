#include "defs.h"

extern void superblock_store(struct superblock sb);
extern struct superblock superblock_load();
extern int balloc(uint);
extern void bfree(uint, uint);

void 
syscall()
{
	if (cpu.proc->tf->eax == 0x10000) {
		uint bias = 0;
		uint off = 6 + bias;
		uint n = 1;
		for (uint i = off; i < off + n; i++)
			bfree(ROOTDEV, i);
	}

	if (cpu.proc->tf->eax == 0x10001) {
		uint block_loc = 0;
		uint pre = 0;
		while (1) {
			block_loc = balloc(ROOTDEV);
			if (block_loc == -1) {
				cprintln(&pre, TYPE_HEX);
				panic("debug");	
			}
			pre = block_loc;
		}
	}

	// syscall test	
	if (cpu.proc->tf->eax == 0x10001) {
		struct superblock sb;
		sb.n_inode_block = 1;
		sb.n_bmap_block = 3;
		sb.n_data_block = 4096 + 4096 + 1024;
		sb.n_block = 2 + sb.n_inode_block + sb.n_bmap_block + sb.n_data_block;
		superblock_store(sb);
	}	

}	
