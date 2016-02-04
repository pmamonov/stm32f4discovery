#ifndef _QUEUE_H
#define _QUEUE_H

#include "FreeRTOS.h"
#include "task.h"

struct queue {
	int in, out, len, cap, sz;
	void *ptr;
	int sem;
};

void queue_init(struct queue *q, int sz, int cap, void *ptr);
int queue_push(struct queue *q, void *ptr);
int queue_pop(struct queue *q, void *ptr);
int queue_swap(struct queue *q1, struct queue *q2);

#endif /* _QUEUE_H */
