#include "queue.h"

//Null terminated queue

void init_threads(queue *t) {
    t->head = NULL;
    t->tail = NULL;
    t->count = 0;
}

void enqueue(queue *t, dthread *td) {
    node *ptr = (node *)malloc(sizeof(node));
    //assigning the new node details
    ptr->td = td;
    ptr->next = NULL;
    //checking if the queue is empty, if empty head and tail points to ptr
    if(t->count == 0) {
        t->head = t->tail = ptr;
    }

    //otherwise append the node to last changing the tail pointer to ptr
    else {
        t->tail->next = ptr;
        t->tail = ptr;
    }
    //increment the count
    t->count = t->count+ 1;
    return;
}

dthread* dequeue(queue *t) {
    //check if the queue is empty
     if(t->count == 0 ) {
        return NULL;
     }
    dthread *td = t->head->td;
    t->head = t->head->next;
    t->count = t->count - 1;
    //check if the new head is null or not , if yes then change the tail
    if(t->head == NULL) {
        t->tail = NULL;
    }
    return(td);
}

void show(queue *t) {
    node *ptr;
    if(t->head == NULL) {
        printf("\nEmpty");
        return;
    }
    ptr = t->head;
    while(ptr != NULL) {
        printf("\nTID: %ld", ptr->td->tid);
        ptr = ptr->next;
    }
    printf("\nTOTAL: %d",t->count);
}

dthread* get_node_by_tid(queue *t, dthread_t tid) {
    node *temp;
    dthread *td;
    if(t->head == NULL) {
        return NULL;
    }
    temp = t->head;
    while(temp != NULL) {
        if(tid == temp->td->tid) {
            return temp->td;
        }
        else {
            temp = temp->next;
        }
    }
    return NULL;
}

//testing the queue data structure

// int main() {
//     dthread *t,*t1;
//     static queue *threads;
//     threads = (queue *)malloc(sizeof(queue));
//     init_threads(threads);
//     t = (dthread *) malloc(sizeof(dthread));
//     t->tid = 23;      //assigning the thread_id
//     enqueue(threads, t);
//     show(threads);
//     dequeue(threads);
//     show(threads);
//     enqueue(threads, t);
//     t1 = get_node_by_tid(threads,23);
//     printf("\n\n%d\n",t1->tid);

// }