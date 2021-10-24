// Simple PIO−based (non−DMA) IDE driver code.
// Only channel 1 is used.

#include "defs.h"

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

/* shared */
// cache ide sectors' data by LRU
struct {
	struct ide_buf bufs[IDE_BUFS];
	struct ide_buf head;
} ide_cache;

/* shared */
static struct ide_buf *ide_queue;	// queue of ide r/w requests


// wait until ide is ready
// set checkerr 1 to check error
static int
ide_wait(int checkerr)
{
	uchar r;
	while (((r = inb(0x1f7)) & (IDE_STAT_BUSY | IDE_STAT_DRDY)) != IDE_STAT_DRDY) {
		uint i = (uint) r;
		cprintf(&i, TYPE_HEX);
	}
	if (checkerr && (r & (IDE_STAT_DF|IDE_STAT_ERR)) != 0) 
		return -1;
	return 0;
}

static void
ide_start(struct ide_buf *p)
{
	ide_wait(0);
	outb(IDE_CTRL_PORT, 0);	// generate intr
	outb(IDE_CNT_PORT, 1);	// read 1 sector
	outb(IDE_POS0_PORT, p->sector & 0xFF);
	outb(IDE_POS1_PORT, (p->sector >> 8) & 0xFF);
	outb(IDE_POS2_PORT, (p->sector >> 16) & 0xFF);
	outb(IDE_POS3_PORT, ((p->sector >> 24) & 0x0F) | IDE_MASTER);
	// write | read
	if (p->flags & IDE_BUF_DIRTY) {
		outb(IDE_COMD_PORT, IDE_COMD_WRITE);
		outsl(IDE_DATA_PORT, p->data, 512/4);
	} else {
		outb(IDE_COMD_PORT, IDE_COMD_READ);
	}
}

/* init */
void 
ide_init()
{
	// initialize ide_cache
	ide_cache.head.prev = &ide_cache.head;
	ide_cache.head.next = &ide_cache.head;
	for (struct ide_buf *p = ide_cache.bufs; p < ide_cache.bufs + IDE_BUFS; p++) {
		p->next = ide_cache.head.next;
		ide_cache.head.next->prev = p;
		p->prev = &ide_cache.head;	
		ide_cache.head.next = p;
		p->dev = -1;
	}			
	
	// enable ide intr
	pic_enable_irq(IRQ_IDE);
	
	// check if the master drive is present
	ide_wait(0);
	outb(IDE_POS3_PORT, IDE_MASTER);	
	for (uint i = 0; i < 1000; i++) {
		if (inb(IDE_STAT_PORT) != 0)
			return;
	}
	panic("ide_init");	// master not found
}

void
ide_intr() 
{
	// ide_queue = ide_queue->next;
	if (!(ide_queue->flags & IDE_BUF_DIRTY) && ide_wait(1) != 0) 
		insl(IDE_DATA_PORT, ide_queue->data, 512/4);
	
	ide_queue->flags |= IDE_BUF_VALID;	// set flag:valid
	ide_queue->flags &= ~IDE_BUF_DIRTY;	// clear flag:dirty
	
	// start if queue is not empty
	ide_queue = ide_queue->next;
	if (ide_queue != NULL)
		ide_start(ide_queue);
}

static void
ide_sync(struct ide_buf *p)
{
	if (!(p->flags & IDE_BUF_BUSY))
		panic("ide_sync");
	if ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) == IDE_BUF_VALID)
		panic("ide_sync");
	if ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) == IDE_BUF_DIRTY)
		panic("ide_sync");

	// attach p to the tail of ide_queue
	p->qnext = NULL;
	if (ide_queue == NULL) {
		ide_queue = p;
		ide_start(p);
	} else {
		struct ide_buf *q;
		for (q = ide_queue; q->qnext; q = q->qnext);
		q->next = p;
	}
}

/* critical */
struct ide_buf *
ide_bget(uint dev, uint sector)
{
	struct ide_buf *p;
	for (p = ide_cache.head.next; p != &ide_cache.head; p = p->next) {
		if (p->dev == dev && p->sector == sector) {
			if (!(p->flags & IDE_BUF_BUSY)) {
				p->flags |= IDE_BUF_BUSY;
				return p;
			} else {
				return NULL;
			}
		}
	}

	// not found in ide_cache
	// recycle a LRU buf
	for (p = ide_cache.head.prev; p != &ide_cache.head; p = p->prev) {
		if (!(p->flags & IDE_BUF_BUSY)) {
			p->flags |= IDE_BUF_BUSY;
			if (p->flags & IDE_BUF_DIRTY)	{
				ide_sync(p);
				// wait for buf's ready
				while ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) != IDE_BUF_VALID);
			}
			p->dev = dev;
			p->sector = sector;
			p->flags = IDE_BUF_BUSY;
			ide_sync(p);
			while ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) != IDE_BUF_VALID);
			return p;
		}
	}
	return NULL;
}

/* critical */
void
ide_brelse()
{

}
