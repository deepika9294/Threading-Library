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
#include <linux/futex.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <signal.h>


#define THREAD_STACK_SIZE (1024 * 64)

#define JOINABLE 1
#define JOINED 2
#define SIGNAL 3

typedef unsigned long int dthread_t;
typedef unsigned int dthread_spinlock_t;
typedef unsigned int dthread_mutex_t;

typedef struct dthread {
    dthread_t tid;
    int state; //joinable, joined
    char *stack;
    void *(*start_routine)(void *);
    void *args;
    void* retval;   
    
} dthread;

// for clone
int fn(void *arg);

void dthread_init(void);
int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args);
void dthread_exit(void *retval);
dthread_t dthread_self(void);
int dthread_join(dthread_t thread, void **retval);
int dthread_kill(dthread_t thread, int sig);
void dthread_cleanup(void);
//temp function for debugging
void show1();


//spin lock
int dthread_spin_init(dthread_spinlock_t *lock);
int dthread_spin_lock(dthread_spinlock_t *lock);
int dthread_spin_trylock(dthread_spinlock_t *lock);
int dthread_spin_unlock(dthread_spinlock_t *lock);

//mutex_lock

int dthread_mutex_init(dthread_mutex_t *mutex);
int dthread_mutex_lock(dthread_mutex_t *mutex);
int dthread_mutex_unlock(dthread_mutex_t *mutex);

