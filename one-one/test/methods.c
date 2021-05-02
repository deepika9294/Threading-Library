#include "../dthread.h"
#include <stdint.h>

dthread_t t1[10];
static int check = 0;
static int counter = 0;
static int signal_counter = 0;
int run = 0;


int testing(int a, int b) {

    if(a == b) {
        return 1;   //if passed
    }
    return 0;   //if failed
}

void* add(void *args) {
    // addition of two numbers
    int a = 89;
    int b = 11;
    return (void *)(intptr_t)(a+b);
}

void* square(void *args) {
    int n = (intptr_t)args;
    return (void *)(intptr_t)(n*n);
}

void* deadlock(void *args) {
    
    int d  = dthread_join(t1[2], NULL);

    check = testing(d,EDEADLK);
    if(check == 0) {
        printf("**FAILED**: Deadlock Join test\n");
    }
    else {
        printf("**PASSED**: Deadlock join test\n");
    }
    return (void *)(intptr_t)(d);

}

void* exit_test(void *args) {
    dthread_exit((void *)(intptr_t)100);
    return (void *)(intptr_t)0;
}

void* exit_control(void *args) {
    counter++;
    dthread_exit(NULL);
    counter++;
    return NULL;
}

void* looping(void *args) {
    while(1);
    return NULL;
}

void* cont(void *args) {
    signal_counter++;
    return NULL;

}

void* sig_loop(void *args) {
    while(run);
    return (void *)42;
}

void signal_handler(int sig) {
    // printf("Inside handler %d\n", sig);
    run = 0;
    return;
}

int main() {

    //for getting return values
    void *tret;
    signal(SIGUSR2, signal_handler);

    printf("\n\n-----------------ALL THE METHODS TESTING (CREATE, JOIN, EXIT, KILL)--------------\n\n");
	dthread_init();

	int c1 = dthread_create( &t1[0], add , NULL);
    int j1 = dthread_join(t1[0], &tret);
    if( c1 == 0 && j1 == 0 ) {
        check = testing((intptr_t)tret, 100);
        if(check == 0) {
            printf("**FAILED**: Addition test with create and join return value\n");
        }
        else {
            printf("**PASSED**: Addition test with create and join return value\n");
        }
    }
    else {
        printf("*****Issue in creating or joining thread *****\n");
    }

    c1 = dthread_create( &t1[1], add , NULL);
    j1 = dthread_join(25, &tret);
    if(c1 == 0) {
       check = testing(j1,ESRCH);
       if(check == 0) {
            printf("**FAILED**: Joining an invalid thread test\n");
        }
        else {
            printf("**PASSED**: Joining an invalid thread test\n");
        }
    }

    j1 = dthread_join(t1[0], NULL);
    check = testing(j1, EINVAL);
    if(check == 0) {
        printf("**FAILED**: Joining on already joined thread test\n");
    }
    else {
        printf("**PASSED**: Joining on already joined thread test\n");
    }


    c1 = dthread_create( &t1[2], deadlock , NULL);

    
    c1 = dthread_create( &t1[3], exit_test, NULL);
    j1 = dthread_join(t1[3], &tret);

    if(c1 == 0) {
        check = testing((intptr_t)tret, 100);
        if(check == 0) {
            printf("**FAILED**: Thread exit test with retval\n");
        }
        else {
            printf("**PASSED**: Thread exit test with retval\n");
        }
    }
    
    c1 = dthread_create( &t1[4], exit_control, NULL);
    j1 = dthread_join(t1[4], &tret);
    if(c1 == 0) {
        check = testing(counter, 1);
        if(check == 0) {
            printf("**FAILED**: Thread exit test, i.e rest of the instructions are not executed\n");
        }
        else {
            printf("**PASSED**: Thread exit test, i.e rest of the instructions are not executed\n");
        }
    }

    c1 = dthread_create( &t1[8], sig_loop, NULL);
    int s = dthread_kill(t1[8], SIGUSR2);
    dthread_join(t1[8],&tret);

    if(c1 == 0) {
        check = testing((intptr_t)tret, 42);
        if(check == 0) {
            printf("**FAILED**: Signal Handler test case for infinite loop\n");
        }
        else {
            printf("**PASSED**: Signal Handler test case for infinite loop\n");
        }
    }


    c1 = dthread_create( &t1[5], looping, NULL);
    // j1 = dthread_join(t1[5], &tret);
    int k = dthread_kill(t1[5], -20);
    if(c1 == 0) {
        check = testing(k, EINVAL);
        if(check == 0) {
            printf("**FAILED**: Invalid Signal Test\n");
        }
        else {
            printf("**PASSED**: Invalid Signal Test\n");
        }
    }
    k = dthread_kill(34, SIGUSR1);
    if(c1 == 0) {
        check = testing(k, ESRCH);
        if(check == 0) {
            printf("**FAILED**: Thread kill test on invalid thread\n");
        }
        else {
            printf("**PASSED**: Thread kill test on invalid thread\n");
        }
    }
    k = dthread_kill(t1[5], SIGTSTP);
    
    if(c1 == 0) {
        check = testing(k, 0);
        if(check == 0) {
            printf("**FAILED**: Valid Signal Test : SIGTSTP\n");
        }
        else {
            printf("**PASSED**: Valid Signal Test : SIGTSTP\n");
        }
    }

    c1 = dthread_create( &t1[6], cont, NULL);
    //stopping the thread
    k = dthread_kill(t1[6], SIGTSTP);
    //restarting again
    int k2 = dthread_kill(t1[6], SIGCONT);
    sleep(1);
    if(c1 == 0) {
        check = testing(signal_counter, 1);
        if(check == 0) {
            printf("**FAILED**: SIGCONT handling test\n");
        }
        else {
            printf("**PASSED**: SIGCONT handling test\n");
        }
    }
    j1 = dthread_join(t1[5], &tret);
    j1 = dthread_join(t1[6], &tret);

    c1 = dthread_create( &t1[7], square, (void *)(intptr_t)(4));
    j1 = dthread_join(t1[7], &tret);
    if(c1 == 0 && j1 == 0) {
        check = testing((intptr_t)tret, 16);
        if(check == 0) {
            printf("**FAILED**: Creating threads with args and join return value test\n");
        }
        else {
            printf("**PASSED**: Creating threads with args and join return value test\n");
        }
    }
    printf("\n--------------------------------EXITING METHODS TEST--------------------------------\n\n");
    
    dthread_exit(NULL);
    return 0;
}