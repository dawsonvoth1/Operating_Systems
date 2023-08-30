// Using Circular Queue for Shared Array:
#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

// Queue node struct
typedef struct n {
	void *value;
} node;

// Queue struct
typedef struct q {
	node *data;
	int size;
	int front;
	int rear;
} queue;

void enqueue(queue *q, void* value);

void* dequeue(queue *q);

queue* init_queue(queue *q, int size);

void free_queue(queue *q);

int isFull(queue *q);

int isEmpty(queue *q);

#endif