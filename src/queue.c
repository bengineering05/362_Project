#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void init_queue(Queue *q) {
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}

int is_empty(Queue *q) {
    return q->size == 0;
}

int is_full(Queue *q) {
    return q->size == MAX_SIZE;
}

void enqueue(Queue *q, int value) {
    if (is_full(q)) {
        printf("Queue overflow!\n");
        exit(1);
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % MAX_SIZE;
    q->size++;
}

int dequeue(Queue *q) {
    if (is_empty(q)) {
        printf("Queue underflow!\n");
        exit(1);
    }
    int value = q->data[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->size--;
    return value;
}
