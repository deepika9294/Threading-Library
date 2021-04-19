#include "../dthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

dthread_t tid[3];
int counter;
dthread_mutex_t lock;
  
void* func(void* arg)
{
    dthread_mutex_lock(&lock);
    //a method to get which thread is running
    counter += 1;

    printf("\n Start: Thread %d \n", counter);
    //critical section
    sleep(2);
    printf("\n Finish: Thread %d \n", counter);
  
    dthread_mutex_unlock(&lock);
    return NULL;
}
  
int main(void)
{
    int i = 0;
    int error;
    dthread_init();
    dthread_mutex_init(&lock);

    //error checking for mutex
    if (dthread_mutex_init(&lock) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    while (i < 3) {
        error = dthread_create(&(tid[i]),&func, NULL);
        if (error != 0)
            printf("\nError in creation");
        i++;
    }
    dthread_join(tid[0], NULL);
    dthread_join(tid[1], NULL);
    dthread_join(tid[2], NULL);
  
    return 0;
}