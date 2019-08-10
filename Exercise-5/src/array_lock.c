#include "lock.h"
#include "../common/alloc.h"

__thread int mySlotIndex;

struct lock_struct {
	int *flag;
	int tail;
	int size;
};


lock_t *lock_init(int nthreads)
{
	lock_t *lock;

	XMALLOC(lock, 1);

	XMALLOC(lock->flag, nthreads);
	lock->size = nthreads;
	lock->flag[0] = 1;
	lock->tail = 0;

	return lock;
}

void lock_free(lock_t *lock)
{
	XFREE(lock);
}

void lock_acquire(lock_t *lock)
{
	int slot = __sync_fetch_and_add(&lock->tail, 1) % (lock->size);
	mySlotIndex = slot;
	while(!(lock->flag[slot])) { };

}

void lock_release(lock_t *lock)
{
	int slot = mySlotIndex;
	lock->flag[slot] = 0;
	lock->flag[(slot+1) % (lock->size) ] = 1;
}
