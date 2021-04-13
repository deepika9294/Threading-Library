#include <stdio.h>
#include <stdlib.h>
#include "singlyll.h"

void init_threads(list *t) {
    printf("Essnter");

    t->count = 0;
    t->tail = NULL;
    t->head = NULL;
}

void insert_beg(list *t, dthread *td) {
    printf("Essnter");

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
    printf("Enter");
    node *ptr;
    if(t->head == NULL) {
        printf("\nEmpty");
        return;
    }
    ptr = t->head;
    while(ptr != NULL) {
        printf("\nTID: %d", ptr->td->tid);
        ptr = ptr->next;
    }
    printf("\nTOTAL: %d",t->count);
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