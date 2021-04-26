#include "queue.h"


static queue *threads;
static pid_t main_thread_pid; //check if needed



void dthread_init(void) {
    threads = (queue *)malloc(sizeof(queue));
    main_thread_pid = getpid();
    init_threads(threads);
}

int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args) {

    /*
    *   Need to block the signal while creation
    *   Implement a timer
    *   Implement a scheduler
    */

    //malloc a thread
    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));

    t->tid = threads->count;
	t->args = args;
    t->status = READY;
	t->start_routine = start_routine;
	t->next_tid = -1;
    t->signal = -1;
    t->state = JOINABLE;

    struct rlimit rlim;
    getrlimit(RLIMIT_STACK, &rlim);
    //rlim.rlim_cur;
    // printf("Stack: %d", t->stack);

    // Initialise the stack

    stack_t s;
    s.ss_size = rlim.rlim_cur;
    s.ss_flags = 0;
    //mallocing the memory;
    s.ss_sp = malloc(rlim.rlim_cur);
    t->stack = s.ss_sp;

    //call the funtion, which will execute the main

    enqueue(threads, t);

    //unblock the signal

    return 0;

}






int dthread_spin_init(dthread_spinlock_t *lock) {
    *lock = 0;
    return 0;    
}
int dthread_spin_lock(dthread_spinlock_t *lock){
    while(__sync_lock_test_and_set(lock,1));
	return 0;

}
//returning EBUSY when lock is being held by other thread 
int dthread_spin_trylock(dthread_spinlock_t *lock) {
    if(__sync_lock_test_and_set(lock,1)) {
        return EBUSY;
    }
    else {
        return 0;
    }
}
int dthread_spin_unlock(dthread_spinlock_t *lock) {
    if(*lock == 1) {
        __sync_lock_release(lock,0);
        return 0;
    }
    return EINVAL;
}


void show1() {
    show(threads);
   
}