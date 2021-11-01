// content of log blocks
// one in memory, one in disk.
// log & dst in the same dev !!

struct log_header {
	int n;	// how many log_blocks to install ?
	int sector[LOG_BLOCKS];	// dst blocks ---- 
};

// metadata of log blocks.
// in memory

struct log {
	lock lock;	// lw_lock
	int start;	// the sector where resides the first log block.
	int size;		// upper limit of #log_block
	int intrans;	// is it locked?
	int dev;	// which dev do the log blocks store
	struct log_header lh;	//
};

struct log log;

void
log_init()
{
	struct superblock sb;
	lock_init(&log.lock);
	read_superblock(ROOTDEV, &sb);
	log.start = sb.size - sb.nlog;	// start = BLOCKS - LOG_BLOCKS 10 - 3 = 7 
	log.size = sb.nlog;							// (0-6) 7:log_header (8-9):log_blocks
	recover_from_log();			
}

// note that log blocks & dst blocks should be in the same dev
// log_header.sector[] records the sectors where dst blocks reside
// log_header.n denotes how many dst blocks there are.

static void
trans_install()
{
	for (int i = 0; i < log.lh.n; i++) {
		struct buf *lbuf = ide_bget(log.dev, log.start + i + 1, IDE_RO);
		struct ide_buf *dbuf = ide_bget(log.dev, log.lh.sector[i], IDE_RW);
		memmove(dbuf->data, lbuf->data, BLOCK_SIZE);	
		brelse(lbuf);
		brelse(dbuf);
	}
}

// log_write: write dst block in memory first to 
// log blocks in disk. (next step, install log blocks
// to dst blocks' home)

//    log header (memory)
//-------------------------+
//
// 		   n = 0
// 
//  secotr : NULL
//
//--------------------------+
//					^
//					|		
//					|	lh_read		
//					|
//    log header (disk)
//-------------------------+
//
// 		   n = 3
// 
//  secotr : 4, 3, 6
//
//--------------------------+
//
//	log data blocks :
// [4-data] [3-data] [5-data]		
//
//--------------------------+



// ---------+
// log write
// ---------+

// write buf to log blocks instead of to 
// dst blocks.

// you need to write several bufs to disk in
// a single transaction. whenever you want to
// sync a buf, you first write it to log blocks
// & update in-memory log header via log_write().

// later, you want commit all the bufs to dst.
// then, you update the in-momery log heaeder to 
// in-disk log header. if the update fails, nothing big
// deal, because we won't install log to dst anyway.
// if success, then we use trans_install() to sync dst with
// log. if install fails, nothing big deal. cause as long as 
// we don't modify the log block parts, we can install those
// a second time.
// only after the installation is successful, we reset lh.n 0
// first in memory and later in disk. (use write_head()).

void 
log_write(struct ide_buf *p)
{
	// find the log location of p --> index i
	for (int i = 0; i < log.lh.n; i++) {
		if (log.lh.sector[i] == p->sector)
			break;
	}

	// write buf to log blocks
	struct buf *lbuf = bread(p->dev, log.start+i+1, IDE_RW);
	memmove(lbuf->data, p->data, BLOCK_SIZE);
	brelse(lbuf);
	// update log header (in memory)
	log.lh.sector[i] = b->sector;
	log.lh.n++;
}


//-------------+
// How it works
//-------------+

// 1. During a transaction we need to write the following blocks in dev 3: b3, b5, b8
// 2. You turn to log_write(): write buf to log blocks and update the in-memory header. For example, you write b3 to the first log block and update lh.n to 1. then you write b5 and update it to 2 ...
// 3. Aftering you have finished the above updates you sync in-disk header with in-memory header. 
// 4. trans_install(): Aftering setting up log blocks of a transaction successfully now you install the data in log blocks to its destination: data blocks. 
// 5. Aftering installation, you mark in-disk lh.n 0 to prepare for the next round of transaction.

//----------------+
// How it recovers
//----------------+


// 1. First of all, you need to get to the point that the atomic requirement of a transaction: you can either casterall the required writes or caster none of them. Assume that you encounter a failure before you caster all the writes, then you must break the invariant character of your data in disk.
// 2. If you encounter any failure during log_write()s, it's nothing big deal as you have not update header.n which means that you caster none of them.
// 3. If you encouter any failure during trans_install(), it's also nothing big deal as you restart OS, it will run the recover_from_log to redo the trans_install() that had not been executed correctly. 
// 4. If you encouter any failure setting or resetting log_header, it's still nothing big deal. It leeaves the OS the same states as case 2 or case 3 which I have demonstrated already.






//-------------+
// trans_commit
//-------------+

void
trans_commit()
{
	if (log.lh.n > 0) {
		write_head();			
		trans_install();
		log.lh.n = 0;
		write_head();
	}

	// wake up any process waiting to start a transaction
	acquire(&log.lock);
	log.intrans = 0;
	wakeup(&log);
	release(&log.lock);
}

//--------------+
// trans_begin
//--------------+

void
trans_begin()
{
	acquire(&log.lock);		// lock for two reasons: 1. it avoids racing. 2. it avoids lost wake up.
	while (log.intrans == 1) // if not available
		sleep(&log);				// if -> while for two reasons. 1. the first waked process won't always operate with intrans
	release(&log.lock);		// equaling to 0 because it might be changed by a new process that had not been forced to 
}												// sleep. 2. the second & its later waked process must check out the state of intrans.



