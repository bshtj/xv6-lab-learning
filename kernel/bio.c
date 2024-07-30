// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  //  struct buf head;
  struct buf buckets[NBUCKET];
  struct spinlock bucketslock[NBUCKET];
  struct spinlock hashlock;
  int size;     // record the count of used buf - lab8-2
} bcache;

int getH(uint blockno)
{
	return (blockno % NBUCKET);
} 


void
binit(void)
{
  int i;
  struct buf *b;

  bcache.size = 0;  // lab8-2
  initlock(&bcache.lock, "bcache");
  initlock(&bcache.hashlock, "bcache_hash");    // init hash lock - lab8-2
  // init all buckets' locks  - lab8-2
  for(i = 0; i < NBUCKET; ++i) {
    initlock(&bcache.bucketslock[i], "bcache_bucket");
  }

// lab8-2
//  // Create linked list of buffers
//  bcache.head.prev = &bcache.head;
//  bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
// lab8-2
//    b->next = bcache.head.next;
//    b->prev = &bcache.head;
    initsleeplock(&b->lock, "buffer");
//    bcache.head.next->prev = b;
//    bcache.head.next = b;
  }
}


// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  // lab8-2
  int idx = getH(blockno);
  struct buf *pre, *minb = 0, *minpre;
  uint mintimestamp;
  int i;
  
  // loop up the buf in the buckets[idx]
  acquire(&bcache.bucketslock[idx]);  // lab8-2
  for(b = bcache.buckets[idx].next; b; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bucketslock[idx]);  // lab8-2
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // check if there is a buf not used -lab8-2
  acquire(&bcache.lock);
  if(bcache.size < NBUF) {
    b = &bcache.buf[bcache.size++];
    release(&bcache.lock);
    b->dev = dev;
    b->blockno = blockno;
    b->valid = 0;
    b->refcnt = 1;
    b->next = bcache.buckets[idx].next;
    bcache.buckets[idx].next = b;
    release(&bcache.bucketslock[idx]);
    acquiresleep(&b->lock);
    return b;
  }
  release(&bcache.lock);
  release(&bcache.bucketslock[idx]);

  // select the last-recently used block int the bucket
  //based on the timestamp - lab8-2
  acquire(&bcache.hashlock);
  for(i = 0; i < NBUCKET; ++i) {
      mintimestamp = -1;
      acquire(&bcache.bucketslock[idx]);
      for(pre = &bcache.buckets[idx], b = pre->next; b; pre = b, b = b->next) {
          // research the block
          if(idx == getH(blockno) && b->dev == dev && b->blockno == blockno){
              b->refcnt++;
              release(&bcache.bucketslock[idx]);
              release(&bcache.hashlock);
              acquiresleep(&b->lock);
              return b;
          }
          if(b->refcnt == 0 && b->timestamp < mintimestamp) {
              minb = b;
              minpre = pre;
              mintimestamp = b->timestamp;
          }
      }
      // find an unused block
      if(minb) {
          minb->dev = dev;
          minb->blockno = blockno;
          minb->valid = 0;
          minb->refcnt = 1;
          // if block in another bucket, we should move it to correct bucket
          if(idx != getH(blockno)) {
              minpre->next = minb->next;    // remove block
              release(&bcache.bucketslock[idx]);
              idx = getH(blockno);  // the correct bucket index
              acquire(&bcache.bucketslock[idx]);
              minb->next = bcache.buckets[idx].next;    // move block to correct bucket
              bcache.buckets[idx].next = minb;
          }
          release(&bcache.bucketslock[idx]);
          release(&bcache.hashlock);
          acquiresleep(&minb->lock);
          return minb;
      }
      release(&bcache.bucketslock[idx]);
      if(++idx == NBUCKET) {
          idx = 0;
      }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
extern uint ticks;  // lab8-2

void
brelse(struct buf *b)
{
  int idx;
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  // change the lock - lab8-2
  idx = getH(b->blockno);
  acquire(&bcache.bucketslock[idx]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
// lab8-2
//    b->next->prev = b->prev;
//    b->prev->next = b->next;
//    b->next = bcache.head.next;
//    b->prev = &bcache.head;
//    bcache.head.next->prev = b;
//    bcache.head.next = b;
    b->timestamp = ticks;
  }
  
  release(&bcache.bucketslock[idx]);
}

void
bpin(struct buf *b) {
  int hash = getH(b->blockno);
  acquire(&bcache.bucketslock[hash]);
  b->refcnt++;
  release(&bcache.bucketslock[hash]);
}

void
bunpin(struct buf *b) {
  int hash= getH(b->blockno);
  acquire(&bcache.bucketslock[hash]);
  b->refcnt--;
  release(&bcache.bucketslock[hash]);
}


