#include "singlyll.h"

//global variables
static list *threads;
static pid_t main_thread_pid;

void dthread_init() {

    threads = (list *)malloc(sizeof(list));
    main_thread_pid = getpid();
    init_threads(threads);

}

int fn(void *arg) {

    dthread *t = (dthread *)arg;
    t->retval = t->start_routine(t->args);
    return 0;

}

int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args) {
    // allocate memory
    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));
    if(t == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    int clone_return;
    char *stack_top;

    // by using mmap allocate the stack
    t->stack = mmap(NULL, THREAD_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (t->stack == MAP_FAILED) {
        perror("Mmap error");
        exit(EXIT_FAILURE);
    }
    stack_top = t->stack + THREAD_STACK_SIZE;
    
    t->args = args;
    t->start_routine = start_routine;
    t->state = JOINABLE;
    clone_return = clone(fn, stack_top, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, (void *)t);
    if(clone_return == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }
    t->tid = clone_return;      //assigning the thread_id
    t->pid = getpid();

    insert_beg(threads, t);
    *thread = t->tid;
    return 0;
}

void dthread_exit(void *retval) {
    dthread_t tid = dthread_self();

    //get the node with tid and change its return value;
    dthread *td = get_node_by_tid(threads, tid);
    
    if(main_thread_pid == tid) {
        // printf("arey waah");
        // fetch all the tid and pass it to join function 
        node *ptr;
        if(threads->head == NULL) {
            printf("\nEmpty");
            return;
        }
        ptr = threads->head;
        while(ptr != NULL) {
            // printf("\nTID: %ld,  PID: %d", ptr->td->tid, ptr->td->pid);
            dthread_join(ptr->td->tid, NULL);
            ptr = ptr->next;
        }

        exit(0);
    }
    if(td == NULL) {
        return;
    }
    td->retval = retval;
    exit(0);
}

dthread_t dthread_self(void) {

    dthread_t tid = gettid();
    return tid;

}

int dthread_join(dthread_t thread, void **retval) {

    // function shall suspend execution of the calling thread until the target thread terminates,
    dthread *td = get_node_by_tid(threads, thread);
    dthread_t tid = dthread_self();
    //join calling from same calling function
    if(tid == thread) {
        printf("weird\n");
        return EINVAL;
    }

    if(td == NULL){
        return ESRCH;
    }

    int status, exit_status;
    
    if(td->state == DETACHED || td->state == JOINED) {
        return EINVAL;
    }

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

int dthread_kill(dthread_t thread, int sig) {
    //if sig is 0, no signal is sent
    if(sig == 0) {
        return 0;
    }
    int status = kill(thread,sig);
    return status;

}
//temp function for debugging
void show1() {
    show(threads);
}
