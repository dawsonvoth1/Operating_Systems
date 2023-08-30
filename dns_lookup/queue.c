// Using Circular Queue for Shared Array
#include "queue.h"

int isFull(queue *q) {
	if((q->front == q->rear+1) || (q->front == 0 && q->rear == q->size-1)) return -1;
	return 0;
}

int isEmpty(queue *q){
	if (q->front == -1) return -1;
	return 0;
}

void enqueue(queue *q, void *value) {
	if(isFull(q)) {
		printf("ERROR: Queue is full\n");
		exit(1);
	}
	if (q->front == -1) q->front = 0;
	q->rear = (q->rear + 1) % q->size;
	q->data[q->rear].value = value;
}

void* dequeue(queue *q) {
	void *val;
	if(isEmpty(q)) {
		printf("ERROR: Queue is empty\n");
		exit(1);
	}
	val = q->data[q->front].value;
	if(q->front==q->rear){
		q->front = -1;
		q->rear = -1;
	}
	else{
		q->front = (q->front + 1) % q->size;
	}
	return val;
}

queue* init_queue(queue *q, int queue_size) {
	q->data = malloc(queue_size * sizeof(node));
	q->size = queue_size;
	q->front = -1;
	q->rear = -1;
	return q;
}

void free_queue(queue *q) {
	while(!q->size){
		void * val;
		val = dequeue(q);
		free(val);
	}
	free(q->data);
}