#include <stdio.h>
#include <string.h>
#include "uqueue.h"

static int queue_lock(struct queue *q)
{
	int ret = -1;

	taskDISABLE_INTERRUPTS();
	if (q->sem == 0) {
		q->sem = 1;
		ret = 0;
	}
	taskENABLE_INTERRUPTS();
	return ret;
}

static void queue_unlock(struct queue *q)
{
	taskDISABLE_INTERRUPTS();
		q->sem = 0;
	taskENABLE_INTERRUPTS();
}

void queue_init(struct queue *q, int sz, int cap, void *ptr)
{
	q->sz = sz;
	q->cap = cap;
	q->ptr = ptr;
	q->len = 0;
	q->sem = 0;
}

int queue_push(struct queue *q, void *ptr)
{
	if (queue_lock(q))
		return -1;
	if (q->len >= q->cap) {
		queue_unlock(q);
		return -1;
	}
	memcpy(q->ptr + q->sz * q->in, ptr, q->sz);
	q->in = (q->in + 1) % q->cap;
	q->len += 1;
	queue_unlock(q);
	return 0;
}

int queue_pop(struct queue *q, void *ptr)
{
	if (queue_lock(q))
		return -1;
	if (q->len < 1) {
		queue_unlock(q);
		return -1;
	}
	memcpy(ptr, q->ptr + q->sz * q->out, q->sz);
	q->out = (q->out + 1) % q->cap;
	q->len -= 1;
	queue_unlock(q);
	return 0;
}


int queue_swap(struct queue *q1, struct queue *q2)
{
	struct queue tmp;

	if (queue_lock(q1))
		return -1;
	if (queue_lock(q2)) {
		queue_unlock(q1);
		return -1;
	}

	memcpy(&tmp, q1, sizeof(*q1));
	memcpy(q1, q2, sizeof(*q1));
	memcpy(q2, &tmp, sizeof(*q1));

	queue_unlock(q1);
	queue_unlock(q2);
	return 0;
}
