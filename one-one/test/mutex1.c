/*  mutex test
*/

//Code taken from Abhijit Sir's reseources from Race problems

#include <stdio.h>
#include "../dthread.h"

long c = 0, c1 = 0, c2 = 0, run = 1;
dthread_mutex_t lock;
void *thread1(void *arg) {
	while(run == 1) {
		dthread_mutex_lock(&lock);
        c++;
		dthread_mutex_unlock(&lock);

		c1++;
    }
    return NULL;
}
void *thread2(void *arg) {
	while(run == 1) {
        dthread_mutex_lock(&lock);
        c++;
		dthread_mutex_unlock(&lock);
		c2++;
    }
    return NULL;
}
int main() {
	dthread_t th1, th2; 
    dthread_init();
	dthread_mutex_init(&lock);
    printf("\n\n-------------------------MUTEX LOCK TEST -------------------------\n\n");

	dthread_create(&th1, thread1, NULL);
	dthread_create(&th2, thread2, NULL);
	sleep(2);
	run = 0;
	dthread_join(th1, NULL);
	dthread_join(th2, NULL);
	if(c == c1 + c2) {
		printf("**PASSED**: Mutex lock test for race problem\n");
	}
	else {
		printf("**FAILED**: Mutex lock test for race problem\n");
	}

    return 0;
}