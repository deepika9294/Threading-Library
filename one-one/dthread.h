#define _GNU_SOURCE

#include <setjmp.h>
#include <sys/types.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <sys/mman.h>

#define THREAD_STACK_SIZE (1024 * 128)
#define MAX_THREAD 20

#define JOINABLE 1
#define JOINED 2
#define DETACHED 3

typedef unsigned long int dthread_t;


typedef struct dthread {
    dthread_t tid;
    pid_t pid;
    int state; //detached, joinable, joined
    char *stack;
    void *(*start_routine)(void *);
    void *args;
    void* retval;   //on success returns NULL (create_thread)
    
} dthread;

// for clone
int fn(void *arg);

void dthread_init(void);
int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args);
void dthread_exit(void *return_value);
void dthread_exit(void *retval);
dthread_t dthread_self(void);
int dthread_join(dthread_t thread, void **retval);
int dthread_kill(dthread_t thread, int sig);
//temp function
void show1();
/*
check if the thread has terminanted or not by retval:
fetch the thread from the list and check if it joinable.
on success returns 0 else err no
wait for the thread to terminante.
*/