#include <stdio.h>
#include <stdlib.h> /* rand() */
#include <limits.h>
#include <stdint.h>

#include "../common/alloc.h"
#include "ll.h"

typedef struct ll_node {
	int key;
	struct ll_node *next;
} ll_node_t;

struct linked_list {
	ll_node_t *head;
};

/**
 * Create a new linked list node.
 **/
static ll_node_t *ll_node_new(int key)
{
	ll_node_t *ret;

	XMALLOC(ret, 1);
	ret->key = key;
	ret->next = NULL;

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

ll_node_t *get(ll_node_t *refer, unsigned *marked)
{
	long addr;

	addr = (long) refer;
	*marked = addr & 1;

	return (ll_node_t *) (addr & ~1);
}

void find(ll_node_t **bef, ll_node_t **aft, ll_t *ll, int key)
{
	ll_node_t *curr, *next, *succ;
	int snip;
	unsigned marked;
retry:
	while (1) {
		curr = ll->head;
		next = get(curr->next, &marked);

		while (1) {
			succ = get(next->next, &marked);
			while (marked) {
				snip = __sync_bool_compare_and_swap(&curr->next, next, succ);
				if (!snip)
					goto retry;
				next = succ;
				succ = get(next->next, &marked);
			}

			if (next->key >= key) {
				*bef = curr;
				*aft = next;
				return;
			}
			curr = next;
			next = succ;
		}
	}
}

int ll_contains(ll_t *ll, int key)
{
	ll_node_t *curr = ll->head;
	int ret = 0;
	unsigned marked;

	/* ll_contains doesn't need locks */
	while (curr->key < key) {
		curr = get(curr->next, &marked);
		// Could avoid get but whatever
	}

	ret = (key == curr->key && !marked);
	return ret;
}

int ll_add(ll_t *ll, int key)
{
	ll_node_t *curr, *next, *new_node;
	new_node = ll_node_new(key);

	while (1) {
		find(&curr, &next ,ll, key);
		if (next->key == key) {
			ll_node_free(new_node);
			return 0;
		}
		else {
			new_node->next = next; //marked field will be zero this way
			if ( __sync_bool_compare_and_swap(&curr->next, next, new_node) )
				return 1;
		}
	}
}

int ll_remove(ll_t *ll, int key)
{
	int snip;
	unsigned marked;
	ll_node_t *curr, *next, *succ, *dirty;

	while (1) {
		find(&curr, &next, ll, key);
		if (next->key != key) {
			return 0;
		}
		else {
			succ = get(next->next, &marked);
			dirty = (ll_node_t *) ( (uintptr_t) succ | 1 );
			snip = __sync_bool_compare_and_swap(&next->next, succ, dirty);
			if (!snip) continue;
			__sync_bool_compare_and_swap(&curr->next, next, succ);
			return 1;
		}
	}
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
