#include "dthread.h"

typedef struct node {
    dthread *td;
    struct node *next;
    // struct node *prev;
} node;

typedef struct list {
    struct node *head;
    struct node *tail;
    int count;
} list;

void init_threads(list *t);
int get_count(list *t);
void insert_beg(list *t, dthread *td);
dthread remove_last(list *t);
void show(list *t);

