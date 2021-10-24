#ifndef _IDE_H
#define _IDE_H

#define IDE_BUF_BUSY	0x01	// locked
#define IDE_BUF_VALID	0x02	// has data carried
#define IDE_BUF_DIRTY	0x04	// modified

#define IDE_BUF_SIZE	512

// assume buf(block) takes the size of 512B
struct ide_buf {
	uint flags;	
	uint dev;	// which dev
	uint sector;	// which sector
	struct ide_buf *prev;	// entry for buf_cache
	struct ide_buf *next;	// entry for buf_cache
	struct ide_buf *qnext;	// entry for ide_queue only
	uchar data[IDE_BUF_SIZE];	
};

#endif
