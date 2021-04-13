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


typedef unsigned long int dthread_t;


typedef struct dthread {
    dthread_t tid;
    pid_t pid;
    // int status; //detached or joinable
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