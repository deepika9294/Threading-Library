#include "queue.h"

//global variables, which are accesed by all the methods

static queue *threads;  //list of all the threads
static dthread *td_cur; //detials of thread which is running
struct itimerval timer; 
static dthread_t tid_count = 0; //used to assign thread id's


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

/*
*   Init function: before using any of the other methods user has to call this method
*   It initializes the global threads list and signal.
*   Initializes the main thread details
*   Timer is started for raising sigvtalrm signal which further will call scheduler
*/

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
    td_cur->signal = -1;
    sigsetjmp(td_cur->context, 1);


    //initialising signal for timer
    struct sigaction sa;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGVTALRM);
    //whenever sigvtalrm is raised, scheduler is called
    sa.sa_handler = &scheduler;
    sa.sa_flags = 0;
    sa.sa_mask = mask;
    sigaction (SIGVTALRM, &sa, NULL);
    atexit(dthread_cleanup);
    start_timer(&timer);

}

/*
*   This method is used to clean all the threads before exiting
*/

void dthread_cleanup() {
    dthread *td;
    int count = threads->count;
    for(int i = 0; i < count; i++){
        td = dequeue(threads);
        free(td);
    }
    //final threads remove
    free(threads);
}

/*
*   Wrapper function to run different thread routines based on current thread details
*/

void fn(void) {
    td_cur->retval = td_cur->start_routine(td_cur->args);
    td_cur->status = TERMINATED;
    raise(SIGVTALRM);
    return;
}

/*
*   Whenever sigvtalrm is raised scheduler is called,
*   here current running thread thread is enqueued,
*   and is replaced by new thread details, which is not terminated
*   If no such valid threads found in the queue, program exits
*   Before jumping to the thread context, 
*       we check if the thread has any signals to be raised
*/

void scheduler(int sig) {
    stop_timer(&timer);
    dthread_t temp_tid = td_cur->tid;
    if(sigsetjmp(td_cur->context, 1) == 1) {
        return;
    }
    
    enqueue(threads, td_cur);
    if(td_cur->status == RUNNING) {
        td_cur->status = READY;
    }

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
    if(td_cur->tid == temp_tid) {
        exit(0);
    }
    if(td_cur->signal != -1) {
        raise(td_cur->signal);
        td_cur->signal = -1;
    }

    start_timer(&timer);
    siglongjmp(td_cur->context, 1);

}

/*
*   Starting the timer
*/

void start_timer(struct itimerval *timer) {
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = ALARM;
    timer->it_interval.tv_sec = 0;
    timer->it_interval.tv_usec = ALARM;
    setitimer(ITIMER_VIRTUAL, timer, 0);
}

/*
*   Stopping the timer, which means now no more sigvtalrm is raised
*/

void stop_timer(struct itimerval *timer) {
    timer->it_value.tv_sec = 0;
    timer->it_value.tv_usec = 0;
    timer->it_interval.tv_sec = 0;
    timer->it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, timer, 0);
}

/*
*   For thread stack allocation mmap() is used
*   dthread_t thread value is changed with respective tid for user side
*   Saving the threads context
*   Changing the stack,base pointer and program counter for that thread
*   Adding it to the threads list
*/

int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args) {

    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));

    t->tid = tid_count++;
	t->args = args;
    t->status = READY;
	t->start_routine = start_routine;
    t->signal = -1;
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

    //saving the context and changing the stack pointer, base pointer and program counter
    sigsetjmp(t->context, 1);
    t->context[0].__jmpbuf[6] = i64_ptr_mangle((long int) t->stack + THREAD_STACK_SIZE - sizeof(long int));
    t->context[0].__jmpbuf[1] = i64_ptr_mangle((long int) t->stack + THREAD_STACK_SIZE - sizeof(long int));
	t->context[0].__jmpbuf[7] = i64_ptr_mangle((long int) fn);
    // printf("%p", t->context);

    enqueue(threads, t);

    return 0;
}

/*
*   All error checks are done before waiting for the thread specified to terminate
*   while() with some condition is used for waiting purpose
*   retval passed as argument is modified by the actual retval of the thread
*/

int dthread_join(dthread_t thread, void **retval) {

    //find the node having that particular thread id
    
    dthread *temp;
    if(thread == td_cur->tid) {
        return EDEADLK;
    }
    temp = get_node_by_tid(threads,thread);
    if(temp == NULL) {
        return ESRCH;
    }

    //check if the thread is already joined or not
    if(temp->state == JOINED) {
        return EINVAL;
    }
    temp->state = JOINED;

    //othewise loop until the process is terminated
    while(1) {
        if (temp->status == TERMINATED) {
            break;
        }
    }
    //updating the retval  with the routine return value
    if(retval) {
        *retval = temp->retval;
    }
    return 0;
}

/*
*   Status is changed and retval is updated, also sigvtalrm is raised to get the new thread
*/

void dthread_exit(void *retval) {
    // changing the retval for the current running thread.
    td_cur->retval = retval;
    td_cur->status = TERMINATED;
    
    //raising the signal to call the scheduler 
    raise(SIGVTALRM);
    
}

/*
*   error checking is done
*   see if it the thread id passed is of the current running thread
*   if yes then raise signal, otherwise append it to the threads structure, to raise it later
*/

int dthread_kill(dthread_t thread, int sig) {
    //no signal
    if(sig == 0) {
        return 0;
    }
    //invalid signal
    if(sig < 0 || sig > 64) {
        return EINVAL;
    }

    // check if current thread id is same as the thread passed

    int status = -1;
    if(thread == td_cur->tid) {
        status = raise(sig);
    }

    //otherwise store it into the thread structure to raise it later
    else {
        dthread *temp = get_node_by_tid(threads, thread);
        if(temp != NULL) {
            temp->signal = sig;
            status = 0;
        }
        else {
            status = ESRCH;
        }
    }
   
    return status;
}

/*
*   Spin lock is implemeted with the help of __sync_lock_test_and_set
*   and __sync_lock_release methods
*/

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