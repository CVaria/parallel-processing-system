#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <limits.h>
#include <pthread.h> /* for pthread_spinlock_t */

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
	int key;
	struct ll_node *next;
	pthread_spinlock_t node_lock;
} ll_node_t;

struct linked_list {
	ll_node_t *head;
};

/**
 * Functions to acquire and release spinlock
 **/
void lock_acquire(pthread_spinlock_t *lock)
{
	if ( pthread_spin_lock(lock) ) {
		perror("Spinlock acquire");
		exit(1);
	}
}
void lock_release(pthread_spinlock_t *lock)
{
	if ( pthread_spin_unlock(lock) ) {
		perror("Spinlock release");
		exit(1);
	}
}

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key)
{
	ll_node_t *ret;

	XMALLOC(ret, 1);
	ret->key = key;
	ret->next = NULL;
	if ( pthread_spin_init(&ret->node_lock, PTHREAD_PROCESS_SHARED) ) {
		perror("Spinlock initialization");
		exit(1);
	}

	return ret;
}

/**
 * Free a linked list node.
 **/
static void ll_node_free(ll_node_t *ll_node)
{
	XFREE(ll_node);
}

/**
 * Create a new empty linked list.
 **/
ll_t *ll_new()
{
	ll_t *ret;

	XMALLOC(ret, 1);
	ret->head = ll_node_new(-1);
	ret->head->next = ll_node_new(INT_MAX);
	ret->head->next->next = NULL;

	return ret;
}

/**
 * Free a linked list and all its contained nodes.
 **/
void ll_free(ll_t *ll)
{
	ll_node_t *next, *curr = ll->head;
	while (curr) {
		next = curr->next;
		ll_node_free(curr);
		curr = next;
	}
	XFREE(ll);
}

/**
 * Validate consistency of list
 **/
int validate(ll_t *ll, ll_node_t *curr, ll_node_t *next)
{
	int key = curr->key;
	ll_node_t *temp = ll->head;

	while (temp->key <= key) {
		if (temp == curr)
			return curr->next == next;
		temp = temp->next;
	}

	return 0;
}

int ll_contains(ll_t *ll, int key)
{
	ll_node_t *curr = ll->head;
	int ret = 0;

	/* ll_contains doesn't need locks */
	while (curr->key < key)
		curr = curr->next;

	ret = (key == curr->key);
	return ret;
}

int ll_add(ll_t *ll, int key)
{
	int ret = 0, br = 1;
	ll_node_t *curr, *next;
	ll_node_t *new_node;

	while (br) {
		curr = ll->head;
		next = curr->next;

		while (next->key < key) {
			curr = next;
			next = curr->next;
		}

		lock_acquire(&curr->node_lock);
		lock_acquire(&next->node_lock);

		if ( validate(ll, curr, next) ) {
			if (key != next->key) {
				ret = 1;
				new_node = ll_node_new(key);
				new_node->next = next;
				curr->next = new_node;
			}
			br = 0;
		}

		lock_release(&curr->node_lock);
		lock_release(&next->node_lock);
	}

	return ret;
}

int ll_remove(ll_t *ll, int key)
{
	int ret = 0, br = 1;
	ll_node_t *curr, *next;

	while (br) {
		curr = ll->head;
		next = curr->next;

		while (next->key < key) {
			// if (next->key == key)
			// 	break;
			curr = next;
			next = curr->next;
		}

		lock_acquire(&curr->node_lock);
		lock_acquire(&next->node_lock);

		if ( validate(ll, curr, next) ) {
			if (key == next->key) {
				ret = 1;
				curr->next = next->next;
			}
			br = 0;
		}

		lock_release(&curr->node_lock);
		lock_release(&next->node_lock);
	}

	return ret;
}

/**
 * Print a linked list.
 **/
void ll_print(ll_t *ll)
{
	ll_node_t *curr = ll->head;
	printf("LIST [");
	while (curr) {
		if (curr->key == INT_MAX)
			printf(" -> MAX");
		else
			printf(" -> %d", curr->key);
		curr = curr->next;
	}
	printf(" ]\n");
}
