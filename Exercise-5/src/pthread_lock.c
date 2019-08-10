#include "lock.h"
#include "../common/alloc.h"
#include <pthread.h>

struct lock_struct {
	pthread_spinlock_t spinlock;
};

lock_t *lock_init(int nthreads)
{
	lock_t *lock;

	XMALLOC(lock, 1);
	if ( pthread_spin_init(&lock->spinlock, PTHREAD_PROCESS_SHARED) ) {
		perror("Spinlock initialization");
		exit(1);
	}
	return lock;
}

void lock_free(lock_t *lock)
{
	XFREE(lock);
}

void lock_acquire(lock_t *lock)
{
	if ( pthread_spin_lock(&lock->spinlock) ) {
		perror("Spinlock acquire");
		exit(1);
	}
}

void lock_release(lock_t *lock)
{
	if ( pthread_spin_unlock(&lock->spinlock) ) {
		perror("Spinlock release");
		exit(1);
	}
}
