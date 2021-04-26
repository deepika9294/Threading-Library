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


dthread_t t1, t2, t3;

void* func1(void *args){

	int i = 0;
    // sleep(5);
	while(i < 5){
		printf("%d :Hello World!\n", i);
		i++;
	}
	return NULL;
}	
void* func2(void *args){
	printf("Hi, Sup\n");

    sleep(5);
	printf("Hi, Supss\n");

	return NULL;
}

void* func3(void *args) {
    // sleep(5);
    // int *d;
    // d[1] = 2;
    int a = 2, b = 3;
    printf("Sum is: %d", a+b);
    return (void *)4;
}
int main()
{
    void *tret;
    // atexit(dthread_cleanup);

    pid_t p1;
    p1  = getpid();
    dthread *td;
    printf("PID in main: %d\n", p1);
	dthread_init();
    // for(int i = 0; i < 80000000; i++);
	int a1 = dthread_create( &t1, func1 , NULL);
	int a2 = dthread_create( &t2, func2 , NULL);	
	int a3 = dthread_create( &t3, func3 , NULL);

    show1();
    printf("\nprinting thread details");
   
    
	return 0;
}