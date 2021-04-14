#include <setjmp.h>
#include <sys/types.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <sys/mman.h>
#include "../dthread.h"

void* func1(void *args){

	int i = 0;
    
	while(i < 5){
		printf("%d :Hello World!\n", i);
		i++;
	}
	return NULL;
}	
void* func2(void *args){
	printf("Hi, Sup\n");

    sleep(5);
    dthread_exit(NULL);
	printf("Hi, Supss\n");

	return NULL;
}

void* func3(void *args) {
    sleep(5);
    // int *d;
    // d[1] = 2;
    int a = 2, b = 3;
    printf("Sum is: %d", a+b);
    return (void *)4;
}
int main()
{
	dthread_t t1, t2, t3;
    void *tret;

    pid_t p1;
    p1  = getpid();
    dthread *td;
    printf("PID in main: %d\n", p1);
	dthread_init();
	int a1 = dthread_create( &t1, func1 , NULL);
	int a2 = dthread_create( &t2, func2 , NULL);	
	int a3 = dthread_create( &t3, func3 , NULL);
    int j1 = dthread_join(t1, &tret);	
    dthread_exit(NULL);

    int j2 = dthread_join(t2, &tret);	
    int j3 = dthread_join(t3, &tret);	
    show1();
    // dthread_kill(t2,SIGINT);
    // printf("\nprinting thread details");
    // show(threads);
	// printf("Wow %d,  %d %d, %p", j1,j2,j3, tret);
   
    
	return 0;
}