// Simple PIO−based (non−DMA) IDE driver code.
// Aussme only channel 1 is used.
// Assume only 1 cpu would handle ide_intr.

#include "defs.h"


//--------------------------
//
//    global variables
//
//--------------------------


/* shared */

// cache ide sectors' data by LRU

static struct {
	lock lock;
	struct ide_buf bufs[IDE_BUFS];
	struct ide_buf head;
} ide_cache;


/* shared */

// queue of ide r/w requests

static struct {
	lock lock;
	struct ide_buf *head;	
} ide_queue;


/* shared */

// to avoid 'lost wake up'

static lock lw_lock;


//--------------------------
//
//   function : init 
//
//--------------------------


void 
ide_init()
{
	lock_init(&ide_cache.lock);	// lock initialization
	lock_init(&ide_queue.lock);
	lock_init(&lw_lock);
	ide_cache.head.prev = &ide_cache.head;	// initialize ide_cache
	ide_cache.head.next = &ide_cache.head;
	for (struct ide_buf *p = ide_cache.bufs; p < ide_cache.bufs + IDE_BUFS; p++) {
		p->next = ide_cache.head.next;
		ide_cache.head.next->prev = p;
		p->prev = &ide_cache.head;	
		ide_cache.head.next = p;
		p->dev = -1;
	}			
	pic_enable_irq(IRQ_IDE);	// enable ide intr
	outb(IDE_POS3_PORT, IDE_MASTER);	// check if the master drive is present
	while ((inb(IDE_STAT_PORT) & (IDE_STAT_BUSY | IDE_STAT_DRDY)) != IDE_STAT_DRDY); // wait till ready
	for (uint i = 0; i < 1000; i++) {
		if (inb(IDE_STAT_PORT) != 0) 
			return;
	}
	panic("ide_init");	// master not found
}


//--------------------------
//
//   function : intr
//
//--------------------------


// ide_start will be called exclusively by ide_sync or ide_intr.
// struct buf *p must have been locked.

static void
ide_start(struct ide_buf *p)
{
	while ((inb(IDE_STAT_PORT) & (IDE_STAT_BUSY | IDE_STAT_DRDY)) != IDE_STAT_DRDY); // wait till ready
	outb(IDE_CTRL_PORT, 0);	// generate intr
	outb(IDE_CNT_PORT, 1);	// read 1 sector
	outb(IDE_POS0_PORT, p->sector & 0xFF);
	outb(IDE_POS1_PORT, (p->sector >> 8) & 0xFF);
	outb(IDE_POS2_PORT, (p->sector >> 16) & 0xFF);
	outb(IDE_POS3_PORT, ((p->sector >> 24) & 0x0F) | IDE_MASTER);
	if (p->flags & IDE_BUF_DIRTY) {	// write | read
		outb(IDE_COMD_PORT, IDE_COMD_WRITE);
		outsl(IDE_DATA_PORT, p->data, 512/4);
	} else {
		outb(IDE_COMD_PORT, IDE_COMD_READ);
	}
}


void
ide_intr() 
{
	acquire(&ide_queue.lock);
	if (ide_queue.head == NULL)
		panic("ide_intr: ide queue empty");
	struct ide_buf *p = ide_queue.head;
	ide_queue.head = ide_queue.head->qnext;
	if (ide_queue.head != NULL) {
		ide_start(ide_queue.head);
	}	
	release(&ide_queue.lock);
	
	if (!(p->flags & IDE_BUF_DIRTY)) {	// whether to copy data
		uchar r;
		while (((r = inb(IDE_STAT_PORT)) & (IDE_STAT_BUSY | IDE_STAT_DRDY)) != IDE_STAT_DRDY);
		if ((r & (IDE_STAT_DF|IDE_STAT_ERR)) != 0) 
			panic("ide_intr");
		insl(IDE_DATA_PORT, p->data, 512/4);
	}
	p->flags |= IDE_BUF_VALID;	// set flag:valid
	p->flags &= ~IDE_BUF_DIRTY;	// clear flag:dirty
	
	acquire(&lw_lock);	// wake up proc 
	wakeup(p);
	release(&lw_lock);		
}


//--------------------------
//
//   function : critical
//
//--------------------------


// when calling ide_sync p shall 
// have been locked (IDE_BUF_BUSY).

static void
ide_sync(struct ide_buf *p)
{
	if (!(p->flags & IDE_BUF_BUSY))
		panic("ide_sync: buf not locked");
	if ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) == IDE_BUF_VALID)
		panic("ide_sync: nothing to do");
	if ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) == IDE_BUF_DIRTY)
		panic("ide_sync: not valid");

	acquire(&ide_queue.lock);	// attach p to the tail of ide_queue
	p->qnext = NULL;
	if (ide_queue.head == NULL) {
		ide_queue.head = p;	
		ide_start(p);
	} else {
		struct ide_buf *q;
		for (q = ide_queue.head; q->qnext; q = q->qnext);
		q->next = p;
	}
	release(&ide_queue.lock);

	acquire(&lw_lock);	// sleep until buf's ready
	while ((p->flags & (IDE_BUF_VALID | IDE_BUF_DIRTY)) != IDE_BUF_VALID) {
		sleep(p, &lw_lock);
	}
	release(&lw_lock);
}


//--------------------------
//
//   function : critical
//
//--------------------------


struct ide_buf *
ide_bget(uint dev, uint sector, uint access_mode)
{
	struct ide_buf *p;
	acquire(&ide_cache.lock);
	for (p = ide_cache.head.next; p != &ide_cache.head; p = p->next) {
		if (p->dev == dev && p->sector == sector) {
			if (!(p->flags & IDE_BUF_BUSY)) {
				p->flags |= IDE_BUF_BUSY;
				if (access_mode == IDE_RW)
					p->flags |= IDE_BUF_DIRTY;
				release(&ide_cache.lock);
				return p;
			} else {
				release(&ide_cache.lock);
				return NULL;	// busy now, return or sleep until it's available
			}
		}
	}
	// not found in ide_cache
	// recycle a LRU buf
	for (p = ide_cache.head.prev; p != &ide_cache.head; p = p->prev) {
		if (!(p->flags & IDE_BUF_BUSY)) {
			p->dev = dev;
			p->sector = sector;
			p->flags = IDE_BUF_BUSY;
			release(&ide_cache.lock);	
			ide_sync(p);
			if (access_mode == IDE_RW) 
				p->flags |= IDE_BUF_DIRTY;
			return p;
		}
	}
	release(&ide_cache.lock);
	return NULL;	// no buf available
}


void
ide_brelse(struct ide_buf *p)
{
	if ((p->flags & IDE_BUF_BUSY) == 0)
		panic("ide_brelse: p not locked");
	
	// flush back to drive if dirty
	if (p->flags & IDE_BUF_DIRTY) {
		ide_sync(p);	// thread safe
	}

	acquire(&ide_cache.lock);
	p->prev->next = p->next;	// move buf to the head (RU)
	p->next->prev = p->prev;
	p->next = ide_cache.head.next;
	p->prev = &ide_cache.head;
	ide_cache.head.next = p;
	p->next->prev = p;
	p->flags &= ~IDE_BUF_BUSY;	// clear flag:busy
	release(&ide_cache.lock);
}

