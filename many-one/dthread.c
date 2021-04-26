#include "queue.h"


static queue *threads;
static dthread *td_cur;
struct itimerval timer;


void dthread_init(void) {
    threads = (queue *)malloc(sizeof(queue));
    init_threads(threads);

    //initialising the context for main


    //initialising signal for timer
    struct sigaction sa;
    //whenever sigvtalrm is raised, scheduler is called
    sa.sa_handler = &scheduler;
    sa.sa_flags = 0;
    sigaction (SIGVTALRM, &sa, NULL);
    start_timer(&timer);

}


void fn(void) {
    td_cur->retval = td_cur->start_routine(td_cur->args);
    return;
}


void scheduler(int sig) {
    static int count = 0;
    show(threads);
    printf("HI %d %d\n",++count, sig);
}


void start_timer(struct itimerval *timer) {
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = TIME_INTERVAL;
	timer->it_interval.tv_sec = 0;
	timer->it_interval.tv_usec = TIME_INTERVAL;
    setitimer(ITIMER_VIRTUAL, timer, 0);
}


void stop_timer(struct itimerval *timer) {
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = 0;
	timer->it_interval.tv_sec = 0;
   	timer->it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, timer, 0);
}



int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args) {

    /*
    *   Implement a scheduler
    */

    stop_timer(&timer);

    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));

    //for checking if the timer is raised or not
    // for(int i = 0; i < 80000000; i++);

    t->tid = threads->count;
	t->args = args;
    t->status = READY;
	t->start_routine = start_routine;
	t->next_tid = -1;
    sigemptyset(&t->signal);
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

    if(s.ss_sp == NULL) {
        start_timer(&timer);
        printf("Insufficient res");
        return EAGAIN;  //insufficient resources
    }

    t->stack = s.ss_sp;

    enqueue(threads, t);
    start_timer(&timer);

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