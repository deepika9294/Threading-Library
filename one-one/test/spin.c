/*  SpinLock test
*/

//Code taken from Abhijit Sir's resources from Race problems

#include <stdio.h>
#include "../dthread.h"

#define GREEN "\033[0;32;32m"
#define RED "\033[0;31;31m"
#define NONE "\033[m"

long c = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, run = 1;
dthread_spinlock_t lock;
void *thread1(void *arg) {
	while(run == 1) {
		dthread_spin_lock(&lock);
        c++;
		dthread_spin_unlock(&lock);

		c1++;
    }
    return NULL;
}
void *thread2(void *arg) {
	while(run == 1) {
        dthread_spin_lock(&lock);
        c++;
		dthread_spin_unlock(&lock);
		c2++;
    }
    return NULL;
}
void *thread3(void *arg) {
	while(run == 1) {
        dthread_spin_lock(&lock);
        c++;
		dthread_spin_unlock(&lock);
		c3++;
    }
    return NULL;
}
void *thread4(void *arg) {
	while(run == 1) {
        dthread_spin_lock(&lock);
        c++;
		dthread_spin_unlock(&lock);
		c4++;
    }
    return NULL;
}
int main() {
	dthread_t th1, th2, th3, th4; 
	int a1,a2,a3,a4;
    dthread_init();
    printf("\n\n");

	a1 = dthread_create(&th1, thread1, NULL);
	a2 = dthread_create(&th2, thread2, NULL);
	a3 = dthread_create(&th3, thread3, NULL);
	a4 = dthread_create(&th4, thread4, NULL);
	if(a1 != 0 || a2 != 0 || a3 !=0 || a4 != 0) {
		printf("Error in creation \n");
		return 0;
	}
	sleep(2);
	run = 0;
	dthread_join(th1, NULL);
	dthread_join(th2, NULL);
	dthread_join(th2, NULL);
	dthread_join(th2, NULL);

	if(c == c1 + c2 + c3 + c4) {
		printf(GREEN "**PASSED**: Spin lock test for race problem between 4 threads\n" NONE);
	}
	else {
		printf(RED "**FAILED**: Spin lock test for race problem between 4 threads\n" NONE);
	}

    // printf("\n%ld %ld %ld %ld %ld %ld\n", c, c1+c2+c3+c4 , c1, c2, c3, c4);
    return 0;
}