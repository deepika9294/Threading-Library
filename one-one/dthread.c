#include "dthread.h"



void dthread_init() {
    adt.count = 0;
}


int fn(void *arg) {
    // sleep(10);
    dthread *t = (dthread *)arg;
    // calling the routine.
    if(sigsetjmp(t->env, 0) == 0)
        t->err_return_value = t->start_routine(t->args);
    exit(EXIT_SUCCESS);
}

int dthread_create(dthread_t *thread, void *(*start_routine) (void *), void *args) {
    printf("Creating");
    // allocate memory

    struct dthread *t;
    t = (dthread *) malloc(sizeof(dthread));
    if(t == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    int clone_return;
    char *stack_top;

    //by using malloc
    /*
        t->stack = (char *)malloc(THREAD_STACK_SIZE);
        if(t->stack == NULL) {
        printf("Stack cannot be initialised");
    }
    */

    // by using mmap

    t->stack = mmap(NULL, THREAD_STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (t->stack == MAP_FAILED) {
        perror("Mmap error");
        exit(EXIT_FAILURE);
    }
    stack_top = t->stack + THREAD_STACK_SIZE;
    
    //updating list of all_threads
    
    // all_dthread1.all_dthreads[all_dthread1.dthread_count++] = *t;
    // memcpy(all_dthread1.all_dthreads[all_dthread1.dthread_count], t, sizeof(struct dthread));
    // all_dthread1.dthread_count++;
    t->args = args;
    t->start_routine = start_routine;
    // The arg argument is passed as the argument of the function fn.
    // call a clone system call

    adt.threads[adt.count++] = *t;
    printf("Count: %d\n\n", adt.count);

    clone_return = clone(fn, stack_top, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, (void *)t);
    if(clone_return == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }
    printf("Clone: %d\n", clone_return);
    t->tid = clone_return;      //assigning the thread_id

    //waiting for the execution.
    int status;
    if (wait(&status) == -1) {
        perror("wait");
        exit(1);
    }
    // t->pid = waitpid(t->tid, &status, SIGCHLD);
    printf("PID in func: %d\n", getpid());
    // free(t);
    printf("exiting successfully");
    exit(EXIT_SUCCESS);
}
void* func1(void *args){
	int i = 0;
	while(i < 5){
		printf("%d :Hello World!\n", i);
		i++;
	}
	return args;
}	
void* func2(void *args){
	printf("Hi\n");
	return args;
}
int main()
{
	dthread_t t1, t2;
    pid_t p1;
    p1  = getpid();
    printf("PID in main: %d\n", p1);
	dthread_init();
	int a1 = dthread_create( &t1, func1 , NULL);
	int a2 = dthread_create( &t2, func2 , NULL);	
	// printf("Wow %d,  %d ", a1,a2);
	printf("r");
	return 0;
}


