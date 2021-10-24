#ifndef _IDE_H
#define _IDE_H

#define IDE_BUF_BUSY	0x01	// locked
#define IDE_BUF_VALID	0x02	// has data carried
#define IDE_BUF_DIRTY	0x04	// modified

#define IDE_RO	0x00
#define IDE_RW	0x01

#define IDE_BUF_SIZE	512	// buf size
#define IDE_BUFS 16	// cache size

// control block registers
#define IDE_CTRL_PORT	0x3F6

// command block registers
#define IDE_DATA_PORT 0x1F0
#define IDE_ERR_PORT	0x1F1
#define IDE_CNT_PORT	0x1F2	// how many sectors
#define IDE_POS0_PORT	0x1F3	// bit0 - bit7
#define IDE_POS1_PORT	0x1F4	// bit8 - bit15
#define IDE_POS2_PORT	0x1F5	// bit16 - bit23
#define	IDE_POS3_PORT	0x1F6	// bit24 - bit27
#define IDE_STAT_PORT	0x1F7	// read 
#define IDE_COMD_PORT	0x1F7	// write 

// POS3: bit28 - bit31
#define IDE_MASTER	0xE0
#define IDE_SLAVE		0xF0

// from state port
#define IDE_STAT_BUSY	0x80
#define IDE_STAT_DRDY	0x40
#define IDE_STAT_DF		0x20
#define IDE_STAT_ERR	0x01

// to command port 
#define IDE_COMD_READ		0x20	// read command
#define IDE_COMD_WRITE	0x30	// write command

// assume buf(block) takes the size of 512B
struct ide_buf {
	volatile uint flags;	
	uint dev;	// which dev
	uint sector;	// which sector
	struct ide_buf *prev;	// entry for buf_cache
	struct ide_buf *next;	// entry for buf_cache
	struct ide_buf *qnext;	// entry for ide_queue only
	uchar data[IDE_BUF_SIZE];	
};

#endif
