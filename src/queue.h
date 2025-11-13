#ifndef QUEUE_H
#define QUEUE_H

#define MAX_SIZE 1000

typedef struct {
    int data[MAX_SIZE];
    int front;
    int rear;
    int size;
} Queue;

void init_queue(Queue *q);
int is_empty(Queue *q);
int is_full(Queue *q);
void enqueue(Queue *q, int value);
int dequeue(Queue *q);

#endif