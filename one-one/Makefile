matrix_test : dthread.o singlyll.o matrix.o
			cc -o matrix_test dthread.o singlyll.o matrix.o

spin_test : dthread.o singlyll.o spin.o
			cc -o spin_test dthread.o singlyll.o spin.o

mutex1_test : dthread.o singlyll.o mutex1.o
			cc -o mutex1_test dthread.o singlyll.o  mutex1.o

mutex2_test : dthread.o singlyll.o mutex2.o
			cc -o mutex2_test dthread.o singlyll.o  mutex2.o

methods_test : dthread.o singlyll.o methods.o
			cc -o methods_test dthread.o singlyll.o methods.o
			
dthread.o : dthread.c dthread.h singlyll.h
			cc -c dthread.c

singlyll.o : singlyll.c singlyll.h dthread.h
			cc -c singlyll.c

matrix.o : test/matrix.c dthread.h
			cc -c test/matrix.c

spin.o : test/spin.c dthread.h
			cc -c test/spin.c

mutex1.o : test/mutex1.c dthread.h
			cc -c test/mutex1.c

mutex2.o : test/mutex2.c dthread.h
			cc -c test/mutex2.c

methods.o : test/methods.c dthread.h
			cc -c test/methods.c

clean :
		rm matrix.o singlyll.o dthread.o spin.o mutex1.o mutex2.o methods.o matrix_test spin_test mutex1_test mutex2_test methods_test