/*  mutex test
*/

//Code taken from Abhijit Sir's reseources from Race problems

#include <stdio.h>
#include "../dthread.h"

long c = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, run = 1;
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

void *thread3(void *arg) {
	while(run == 1) {
        dthread_mutex_lock(&lock);
        c++;
		dthread_mutex_unlock(&lock);
		c3++;
    }
    return NULL;
}

void *thread4(void *arg) {
	while(run == 1) {
        dthread_mutex_lock(&lock);
        c++;
		dthread_mutex_unlock(&lock);
		c4++;
    }
    return NULL;
}

int main() {
	dthread_t th1, th2, th3, th4; 
    dthread_init();
	dthread_mutex_init(&lock);
    printf("\n\n--------------------------------MUTEX LOCK TEST ---------------------------------\n\n");
    printf("***** MUTEX TEST : 1 *****\n");

	dthread_create(&th1, thread1, NULL);
	dthread_create(&th2, thread2, NULL);
	dthread_create(&th3, thread3, NULL);
	dthread_create(&th4, thread4, NULL);

	sleep(2);
	run = 0;
	dthread_join(th1, NULL);
	dthread_join(th2, NULL);
	dthread_join(th3, NULL);
	dthread_join(th4, NULL);

	if(c == c1 + c2 + c3 + c4) {
		printf("**PASSED**: MUTEX LOCK test for race problem between 4 threads\n");
	}
	else {
		printf("**FAILED**: MUTEX LOCK test for race problem between 4 threads\n");
	}
    // printf("\n%ld %ld %ld %ld %ld %ld\n", c, c1+c2+c3+c4 , c1, c2, c3, c4);

	printf("\n");
    return 0;
}