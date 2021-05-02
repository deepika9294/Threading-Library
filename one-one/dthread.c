#include "singlyll.h"

//global variables, accessed by all the methods
static list *threads;
static pid_t main_thread_pid;
static dthread_spinlock_t lock;


/*
*   Init function: before using any of the other methods user has to call this method
*   It initializes the global threads list and a lock.
*/
void dthread_init() {

    dthread_spin_init(&lock);

    threads = (list *)malloc(sizeof(list));
    main_thread_pid = getpid();
    init_threads(threads);
}

/*
*   This method is used to clean all the threads before exiting
*/

void dthread_cleanup() {
    dthread_spin_lock(&lock);
    dthread *td;
    
    int count = threads->count;

    for(int i = 0; i < count; i++){
        td = remove_last(threads);
        if(td != NULL)
            free(td);
    }
    //final threads remove
    free(threads);
    dthread_spin_unlock(&lock);

}

/*
*   A wrapper function which is used by clone system call,
*   Takes thread details as the argument and actually execute the routine
*/

int fn(void *arg) {

    dthread *t = (dthread *)arg;
    t->retval = t->start_routine(t->args);
    return 0;

}

/*
*   A thread block is initialised with the details passed.
*   Stack is initialsed for the particular thread with the help of mmap()
*   Threads are created with the help of clone()
*   And the thread information is stored in global threads list
*   dthread_t thread value is changed with respective tid for user side
*/

int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args) {

    dthread_spin_lock(&lock);
    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));
    if(t == NULL) {
        dthread_spin_unlock(&lock);
        return -1;        
    }
    int clone_return;
    char *stack_top;

    // by using mmap allocate the stack
    t->stack = mmap(NULL, THREAD_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (t->stack == MAP_FAILED) {
        dthread_spin_unlock(&lock);
        return EAGAIN;
    }
    
    stack_top = t->stack + THREAD_STACK_SIZE;

    t->args = args;
    t->start_routine = start_routine;
    t->state = JOINABLE;
    clone_return = clone(fn, stack_top, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, (void *)t);
    if(clone_return == -1) {
        dthread_spin_unlock(&lock);
        return EINVAL;
    }
    t->tid = clone_return;      //assigning the thread_id

    insert_beg(threads, t);
    *thread = t->tid;
    dthread_spin_unlock(&lock);
    return 0;
}

/*
*   Terminate the thread, but before that save the return value into the thread structure.
*   If main function is calling exit, it will wait for all the threads to get over first and then terminate main
*/
void dthread_exit(void *retval) {
    dthread_t tid = dthread_self();

    //get the node with tid and change its return value;
    dthread *td = get_node_by_tid(threads, tid);
    
    if(main_thread_pid == tid) {
        // fetch all the tid and pass it to join function 
        node *ptr;
        if(threads->head == NULL) {
            // printf("\nEmpty");
            return;
        }
        dthread_spin_lock(&lock);
        ptr = threads->head;
        dthread_spin_unlock(&lock);

        while(ptr != NULL) {
            if(ptr->td->state == JOINABLE) {
                dthread_join(ptr->td->tid, NULL);
            }
            ptr = ptr->next;
        }
        dthread_cleanup();
        exit(0);
    }
    if(td == NULL) {
        return;
    }
    
    td->retval = retval;
    exit(0);
}

/*
*   Gets the running thread id with the help of gettid()
*/

dthread_t dthread_self(void) {

    dthread_t tid = gettid();
    return tid;

}

/*
*   All error checks are done before waiting for the thread specified to terminate
*   waitpid() is used for waiting purpose
*   retval passed as argument is modified by the actual retval of the thread
*/

int dthread_join(dthread_t thread, void **retval) {

    // function shall suspend execution of the calling thread until the target thread terminates,
    dthread_spin_lock(&lock);
    dthread *td = get_node_by_tid(threads, thread);
    dthread_t tid = dthread_self();
    dthread_spin_unlock(&lock);

    //join calling from same calling function
    if(tid == thread) {
        return EDEADLK;
    }

    if(td == NULL){
        return ESRCH;
    }

    
    if(td->state == JOINED) {
        return EINVAL;
    }
    int status;

    if(td->state == JOINABLE) {
        td->state = JOINED;
        waitpid(thread,&status, 0);
        if(status == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    } 
    //updating the retval
    if(retval) {
        *retval = td->retval;
    }
    return 0; //success
}

/*
*   All Error checking is done before actually raising the signal to the specified thread
*   and status is returned
*/

int dthread_kill(dthread_t thread, int sig) {
    //if sig is 0, no signal is sent
    if(sig == 0) {
        return 0;
    }

    if(sig < 0 || sig > 64) {
        return EINVAL;
    }
    //check if the thread exist
    dthread *temp = get_node_by_tid(threads,thread);

    if(temp == NULL) {
        return ESRCH;
    }
    
    temp->state = SIGNAL;
    int status = kill(thread,sig);
    return status;

}

/*
 * Referred concept from the link: https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html 
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

/*
*Mutex locks
*/

int dthread_mutex_init(dthread_mutex_t *mutex) {
    *mutex = 0;
    return 0;
}

/*
*   Logic of the mutex_lock code is referred from a git repo, link is pasted below:
*   https://github.com/eliben/code-for-blog/blob/master/2018/futex-basics/mutex-using-futex.cpp
*/


/*
* __sync_val_comapre_and_swap()--> compares the value pointed by mutex  with the second parameter
* and if they are equal then mutex points to third parameter value and returns the original value pointed by mutex
*/
int dthread_mutex_lock(dthread_mutex_t *mutex) {
    
    unsigned int temp = __sync_val_compare_and_swap(mutex, 0, 1);
    if(temp != 0) {
        do  {
            if((temp == 2) || __sync_val_compare_and_swap(mutex, 1, 2) != 0)
                syscall(SYS_futex, mutex, FUTEX_WAIT_PRIVATE, 2, 0, 0, 0);
        } while((temp = __sync_val_compare_and_swap(mutex, 0, 2)) != 0);
    }
    return 0;
}


/*
*__sync_fetch_and_sub() --> *mutex = (*mutex - 2nd argument)
function returns the initial value
*
*/

int dthread_mutex_unlock(dthread_mutex_t *mutex) {
    if(__sync_fetch_and_sub(mutex, 1) != 1)  {
    *mutex = 0;
    syscall(SYS_futex, mutex, FUTEX_WAKE_PRIVATE, 1, 0, 0, 0);
   }
   return 0;
}

//temp function for debugging
void show1() {
    show(threads);
   
}
