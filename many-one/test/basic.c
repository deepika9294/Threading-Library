

#include <stdio.h>
#include "../dthread.h"
struct itimerval timer;

void *func1(void *args) {
    //checking if exit is working 
    printf(" Thread 1: Start\n");
    dthread_exit("Exiting");
    return "thread is exiting without explicit exit call";
}

//check multiple of 100
void* func2(void *args) {
    int n = *((int *) args);
    if (n % 100 == 0) {
        return "multiple of 100";
    }
    return "not multiple of 100";
}

void *func3(void *args) {
    //holding for some time to check the concurrency:
    printf("3rd Routine Started\n");
    //will raise alarms and other threads will be scheduled
    for(int i = 0; i < 20000000; i++);
    printf("3rd Routine Ended after waiting for few time\n");
    return NULL;
}


int main()
{
    
    dthread_t t1, t2, t3, t4;
    void *tret;
    int *arg1 = (int *)malloc(sizeof(int));
    *arg1 = 1000;
    int *arg2 = (int *)malloc(sizeof(int));
    *arg2 = 1009;
    dthread_init();

    int check =1000;
    dthread_create(&t1, func1, NULL);
    dthread_create(&t2, func2, arg1);
    dthread_create(&t3, func2, arg2);
    dthread_create(&t4, func3, NULL);



    dthread_join(t1, &tret);
    printf("Return value is:%s\n", (char *)tret);

    dthread_join(t2, &tret);
    printf("Return value is:%s\n", (char *)tret);

    dthread_join(t3, &tret);
    printf("Return value is:%s\n", (char *)tret);

    dthread_join(t4, &tret);
    printf("Return value is:%s\n", (char *)tret);


    dthread_exit(NULL);

    printf("Shouldn't have printed\n");

    return 0;
}