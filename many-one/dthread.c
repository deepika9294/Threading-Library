#include "queue.h"


static queue *threads;
static dthread *td_cur;
struct itimerval timer;
static dthread_t tid_count = 0;


/*
*   snippet is taken from: 
*       https://sites.cs.ucsb.edu/~chris/teaching/cs170/projects/proj2.html    
*/

static long int i64_ptr_mangle(long int p) {
    long int ret;
    asm(" mov %1, %%rax;\n"
        " xor %%fs:0x30, %%rax;"
        " rol $0x11, %%rax;"
        " mov %%rax, %0;"
        : "=r"(ret)
        : "r"(p)
        : "%rax"
    );
    return ret;
}


void dthread_init(void) {
    threads = (queue *)malloc(sizeof(queue));
    td_cur = (dthread *)malloc(sizeof(dthread));
    init_threads(threads);

    //initialising the current thread with main thread details
    td_cur->tid = tid_count++;
    td_cur->status = RUNNING;
    td_cur->state = JOINABLE;
    td_cur->start_routine = NULL;
    td_cur->args = NULL;
    td_cur->retval = NULL;

    sigsetjmp(td_cur->context, 1);


    //initialising signal for timer
    struct sigaction sa;
    sigset_t mask;
	sigfillset(&mask);
    //whenever sigvtalrm is raised, scheduler is called
    sa.sa_handler = &scheduler;
    sa.sa_flags = 0;
    sa.sa_mask = mask;
    sigaction (SIGVTALRM, &sa, NULL);

    start_timer(&timer);

}


void fn(void) {
    td_cur->retval = td_cur->start_routine(td_cur->args);
    td_cur->status = TERMINATED;
    raise(SIGVTALRM);
    //the thread needs to be exited
    return;
}


void scheduler(int sig) {
    stop_timer(&timer);
    if(sigsetjmp(td_cur->context, 1) == 1) {
        // printf("check");
        return;
    }

    enqueue(threads, td_cur);
    if(td_cur->status == RUNNING) {
        td_cur->status = READY;
    }
    // show(threads);

    //getting the next thread details:
    //in loop, to check if the thread is already terminated or not
    dthread *temp;
    int threads_count = threads->count;
    for(int i = 0; i < threads_count; i++) {
        temp = dequeue(threads);
        if(temp->status == READY) {
            td_cur = temp;
            td_cur->status = RUNNING;
            break;
        }
        else if(temp->status == TERMINATED) {
            enqueue(threads, temp);
        }
    }

    start_timer(&timer);
    siglongjmp(td_cur->context, 1);

}


void start_timer(struct itimerval *timer) {
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = ALARM;
	timer->it_interval.tv_sec = 0;
	timer->it_interval.tv_usec = ALARM;
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

    stop_timer(&timer);

    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));

    //for checking if the timer is raised or not
    // raise(SIGVTALRM);

    t->tid = tid_count++;
	t->args = args;
    t->status = READY;
	t->start_routine = start_routine;
    sigemptyset(&t->signal);
    t->state = JOINABLE;


    // allocate the memory for stack with mmap

    t->stack = mmap(NULL,THREAD_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,-1, 0);

    if(t->stack == MAP_FAILED) {
        start_timer(&timer);
        printf("Insufficient res");
        return EAGAIN;  //insufficient resources
    }

    //assigning the thread with the tid
    *thread = t->tid;

    //saving the context and changing the stack pointer and program counter
    sigsetjmp(t->context, 1);
    t->context[0].__jmpbuf[6] = i64_ptr_mangle((long int) t->stack + THREAD_STACK_SIZE - sizeof(long int));
    t->context[0].__jmpbuf[1] = i64_ptr_mangle((long int) t->stack + THREAD_STACK_SIZE - sizeof(long int));
	t->context[0].__jmpbuf[7] = i64_ptr_mangle((long int) fn);
    // printf("%p", t->context);

    enqueue(threads, t);
    start_timer(&timer);
    return 0;
}

int dthread_join(dthread_t thread, void **retval) {

    //find the node having that particular thread
    printf("Thread id%d\n",thread);
    dthread *temp;
    temp = get_node_by_tid(threads,thread);
    if(temp == NULL) {
        return EINVAL;
    }
    //othewise loop until the process is completed
    while(1) {
        if (temp->status == TERMINATED) {
            break;
        }
    }
    if(retval) {
        *retval = temp->retval;
    }

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