#include<stdio.h>
#include<setjmp.h>
#include<assert.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/time.h>
#include<errno.h>
#include<sys/resource.h>


#define THREAD_STACK_SIZE (1024 * 64)
#define MAX_THREAD 20

#define JOINABLE 1
#define JOINED 2
#define DETACHED 3


#define READY 1
#define RUNNING 2
#define TERMINATED 3
#define WAITING 4


typedef unsigned long int dthread_t;
typedef unsigned int dthread_spinlock_t;
typedef unsigned int dthread_mutex_t;

typedef struct dthread {
    dthread_t tid;
    int state;  //joinable joined, detached
    int status; //ready, running,terminated, waiting
    char *stack;    //stack_base address
    void *(*start_routine)(void *);
    void *args;
    void* retval;   //on success returns NULL (create_thread)
    sigjmp_buf context;
    int signal;
    dthread_t next_tid;
} dthread;


int fn(void *arg);  //might not needed

void dthread_init(void);
int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args);
void dthread_exit(void *retval);
dthread_t dthread_self(void);
int dthread_join(dthread_t thread, void **retval);
int dthread_kill(dthread_t thread, int sig);
void dthread_cleanup(void);

void show1();

//spin lock
int dthread_spin_init(dthread_spinlock_t *lock);
int dthread_spin_lock(dthread_spinlock_t *lock);
int dthread_spin_trylock(dthread_spinlock_t *lock);
int dthread_spin_unlock(dthread_spinlock_t *lock);