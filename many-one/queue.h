#include "dthread.h"


typedef struct node {

    dthread *td;
    struct node *next;

} node;

typedef struct queue {

    struct node *head;
    struct node *tail;
    int count;

} queue;

void init_threads(queue *t);
void enqueue(queue *t, dthread *td);
dthread* dequeue(queue *t);
void show(queue *t);
dthread* get_node_by_tid(queue *t, dthread_t tid);
