/*  SpinLock test
*/

//Code taken from Abhijit Sir's reseources from Race problems

#include <stdio.h>
#include "../dthread.h"

long c = 0, c1 = 0, c2 = 0, run = 1;
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
int main() {
	dthread_t th1, th2; 
    dthread_init();
	dthread_spin_init(&lock);
	dthread_create(&th1, thread1, NULL);
	dthread_create(&th2, thread2, NULL);
	fprintf(stdout, "Ending main\n");
	sleep(2);
	run = 0;
	dthread_join(th1, NULL);
	dthread_join(th2, NULL);
	fprintf(stdout, "c = %ld c1+c2 = %ld c1 = %ld c2 = %ld \n", c, c1+c2, c1, c2);
    return 0;
}