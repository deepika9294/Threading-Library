**Implementation of User-Level Multithreading Library**__

**Modes Implemented**

-     One-one Model : Each user thread is mapped to kernel thread
-     Many-one Model : Multiple user threads with one kernel thread. SIGVTALRM signals are used to do the scheduling of user threads. 


**Functions Supported**

    dthread_create()
    dthread_join()
    dthread_exit()
    dthread_kill()
    dthread_self()

**Synchornization methods**

    dthread_spin_lock()
    dthread_spin_unlock()
    dthread_spin_trylock()
    dthread_mutex_lock()
    dthread_mutex_unlock()


**Automated Testing**

To run the automated testing for both the models simultaneously, navigate to the root folder of the project and run, :
    make run

For individual model testing:
    1. For one-one, run:
        cd one-one
        make run
    2. For many-one. run:
        cd many-one
        make run

To clean all the object files created, run:
    make clean
    


