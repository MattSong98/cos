#include "defs.h"

void 
syscall()
{
	// inode_read - bug free
	if (cpu.proc->tf->eax == 0x10001) {
		struct inode *ip = inode_get(ROOTDEV, 2);
		inode_lock(ip);
		char str[1000];
		str[999] = '\0';
		if (inode_read(ip, str, 0x11800-999+999, 999) == 999-999)
			cprintln("hello", TYPE_STR);
		panic("debug");
	}

	// inode_write - bug free 
	if (cpu.proc->tf->eax == 0x10001) {
		uint off = 0;
		struct inode *ip = inode_get(ROOTDEV, 2);
		inode_lock(ip);
		while (inode_write(ip, "@", off, 1) != -1) {
			off++;
			// panic("writing failure");
		}
		cprintln(&off, TYPE_HEX);
		panic("debug");
	}	

	// inode_put - bug free
	if (cpu.proc->tf->eax == 0x10001) {
		struct inode *ip = inode_get(ROOTDEV, 1);
		inode_put(ip);
	}	

	// inode_lock & inode_unlock - bug free
	if (cpu.proc->tf->eax == 0x10001) {
		struct inode *ip = inode_get(ROOTDEV, 9);
		inode_lock(ip);
		// inode_unlock(ip);
		// inode_lock(ip2);
		panic("debug");
	}
	
	// inode_get - bug free 
	if (cpu.proc->tf->eax == 0x10001) {
		for (int i = INODES; i >= 0; i--) {
			struct inode *ip = inode_get(ROOTDEV, i);
			for (int j = 0; j < 0xFF; j++)
				ip = inode_get(ROOTDEV, i);
			cprintln(&ip->dev, TYPE_HEX);
			cprintln(&ip->inum, TYPE_HEX);
			cprintln(&ip->ref, TYPE_HEX);
			cprintln(&ip->lock, TYPE_HEX);
			cprintln(&ip->data.type, TYPE_HEX);
			cprintln(&ip->data.major, TYPE_HEX);
			cprintln(&ip->data.minor, TYPE_HEX);
			cprintln(&ip->data.n_link, TYPE_HEX);
			cprintln(&ip->data.size, TYPE_HEX);
		}
	}

	// inode_alloc (IPB = 7) - bug free 
	if (cpu.proc->tf->eax == 0x10001) {
		for (uint i = 0; i < 15; i++) {
			uint inum = inode_alloc(ROOTDEV, FILE);
			cprintln(&inum, TYPE_HEX);
		}
	}

	// bfree - bug free
	if (cpu.proc->tf->eax == 0x10001) {
		uint bias = 1;
		uint off = 7 + bias;
		uint n = 1;
		for (uint i = off; i < off + n; i++)
			bfree(ROOTDEV, i);
	}

	// balloc - bug free
	if (cpu.proc->tf->eax == 0x10001) {
		for (uint i = 0; i < 1025; i++) {
			balloc(ROOTDEV);
		}
	}
}
