#include <stdio.h>
#include <stdlib.h>
#include "singlyll.h"

void init_threads(list *t) {

    t->count = 0;
    t->tail = NULL;
    t->head = NULL;
}

void insert_beg(list *t, dthread *td) {
    //add condition for maximum
    node *ptr;
    ptr = (node*)malloc(sizeof(node));
    if(ptr == NULL) {
        printf("\nMemory full");
        return;
    }
    ptr->td = td;
    ptr->next = NULL;
    if(t->head == NULL) {
        t->head = ptr;
        t->tail = ptr;
    }
    else {
        ptr->next = t->head;
        t->head = ptr;
    }
    t->count = t->count + 1;
}

void show(list *t) {
    node *ptr;
    if(t->head == NULL) {
        printf("\nEmpty");
        return;
    }
    ptr = t->head;
    while(ptr != NULL) {
        printf("\nTID: %ld,  PID: %d", ptr->td->tid, ptr->td->pid);
        ptr = ptr->next;
    }
    printf("\nTOTAL: %d",t->count);
}

dthread* remove_last(list *t) {
    node *temp;
    dthread *td;
    if(t->tail == NULL) {
        printf("\nEmpty list");
        return NULL;
    }
    temp = t->head;
    while(temp->next != t->tail) {
        temp = temp->next;
    }
    td = t->tail->td;
    t->tail = temp;
    t->tail->next = NULL;
    return td;
}

// int main() {
//     dthread *t;
//     // int t;
//     static list *threads;
//     threads = (list *)malloc(sizeof(list));
//     init_threads(threads);
//     t = (dthread *) malloc(sizeof(dthread));
//     t->tid = 23;      //assigning the thread_id
//     insert_beg(threads, t);
//     show(threads);


// }