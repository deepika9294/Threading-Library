// #include "dthread.h"
#include "singlyll.h"

//global variable
static list *threads;

void dthread_init() {
    // adt.count = 0;
    threads = (list *)malloc(sizeof(list));
    init_threads(threads);
}


int fn(void *arg) {
    // sleep(10);
    dthread *t = (dthread *)arg;
    // calling the routine.
    if(sigsetjmp(t->env, 0) == 0)
        t->err_return_value = t->start_routine(t->args);
    return 0;
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
    
    // all_dthread1.dthread_count++;
    t->args = args;
    t->start_routine = start_routine;


    clone_return = clone(fn, stack_top, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, (void *)t);
    if(clone_return == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }
    printf("Clone: %d\n", clone_return);
    t->tid = clone_return;      //assigning the thread_id

    insert_beg(threads, t);

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
    // exit(EXIT_SUCCESS);
    return 0;
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
	printf("Hi, Sup\n");
	return args;
}

void* func3(void *args) {
    int a = 2, b = 3;
    printf("Sum is: %d", a+b);
    return args;
}
int main()
{
	dthread_t t1, t2, t3;
    pid_t p1;
    p1  = getpid();
    dthread *td;
    printf("PID in main: %d\n", p1);
	dthread_init();
	int a1 = dthread_create( &t1, func1 , NULL);
	int a2 = dthread_create( &t2, func2 , NULL);	
	int a3 = dthread_create( &t3, func3 , NULL);	
    printf("\nprinting thread details");
    show(threads);
	printf("Wow %d,  %d %d", a1,a2,a3);
	printf("r");

    td = remove_last(threads);
    show(threads);
    
	return 0;
}


