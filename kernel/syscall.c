#include "defs.h"


extern uint dirlookup(struct inode *dp, char *name, uint *poff);
extern bool dirlink(struct inode *dp, char *name, uint inum);
extern void mkdir(struct inode *, char *);
extern bool rmdir(struct inode *, char *);
extern struct inode *inode_path(char *path);
extern void lsdir(char *, char *);

void 
syscall()
{
	// lsdir
	if (cpu.proc->tf->eax == 0x10000) {
		char str[64], *p = str;
		lsdir("home/mattsongs", str);
		while (strlen(p) != 0) {
			cprintln(p, TYPE_STR);
			p += strlen(p) + 1;
		}
		panic("done!");
	}	

	// inode_path
	if (cpu.proc->tf->eax == 0x10001) {
		if (chdir("home/mattsong/desktop") == false)
			panic("panic! path not valid");
		struct inode *ip = inode_path(".");
		inode_lock(ip);
		if (rmdir(ip, "tmp") == false)
			panic("panic! rmdir");
		inode_unlock(ip);
		inode_put(ip);
		panic("done!");
	}

	// rmdir
	if (cpu.proc->tf->eax == 0x10001) {
		struct inode *rdir = inode_get(ROOTDEV, ROOTINO);
		inode_lock(rdir);
		if (rmdir(rdir, "dev") == false)
			panic("failure");
		panic("success");
	}

	// dirlookup
	if (cpu.proc->tf->eax == 0x10001) {
		uint off;
		struct inode *rdir = inode_get(ROOTDEV, ROOTINO);
		inode_lock(rdir);
		uint inum = dirlookup(rdir, "..", &off);
		if (inum == 0)
			panic("not found");
		else {
			cprintln(&inum, TYPE_HEX);
			cprintln(&off, TYPE_HEX);
			panic("debug");
		} 
	}

	// mkdir 
	if (cpu.proc->tf->eax == 0x10001) {
		struct inode *rdir = inode_get(ROOTDEV, ROOTINO);
		inode_lock(rdir);
		mkdir(rdir, "dev");
		inode_unlock(rdir);
		inode_put(rdir);
		panic("debug");		
	}

	if (cpu.proc->tf->eax == 0x10001) {
		for (int i = 2; i < 14; i++) {
			struct inode *ip = inode_get(ROOTDEV, i);
			inode_lock(ip);
			// rdir->data.n_link = 0;
			inode_unlock(ip);
			inode_put(ip);
		}
	}

	// initialize fs
	if (cpu.proc->tf->eax == 0x10001) {
		if (inode_alloc(ROOTDEV, DIR) != ROOTINO)
			panic("panic! initialize fs");
		struct inode *rdir = inode_get(ROOTDEV, ROOTINO);
		inode_lock(rdir);
		rdir->data.n_link = 1;
		if (dirlink(rdir, ".", ROOTINO) == false)
			panic("panic! initialize fs");
		if (dirlink(rdir, "..", ROOTINO) == false)
			panic("panic! initialize fs");
		inode_unlock(rdir);
		panic("debug");
	}
	
	// synthesis
	if (cpu.proc->tf->eax == 0x10001) {
		uint inum = inode_alloc(ROOTDEV, FILE);
		cprintln(&inum, TYPE_HEX);
		struct inode *ip = inode_get(ROOTDEV, inum);
		inode_lock(ip);
		for (uint off = 0; off + 14 <= BLOCK_SIZE * MAX_FILE; off += 14)
			if (inode_write(ip, "Hello, world!", off, 14) == -1)
				panic("failed");
		inode_unlock(ip);
		panic("debug");
	}

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
		struct inode *ip = inode_get(ROOTDEV, 0);
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

}
