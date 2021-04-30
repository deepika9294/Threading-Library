matrix_test : dthread.o queue.o matrix.o
			cc -o matrix_test dthread.o queue.o matrix.o

dthread.o : dthread.c dthread.h queue.h
			cc -c dthread.c

queue.o : queue.c queue.h dthread.h
			cc -c queue.c

matrix.o : test/matrix.c dthread.h
			cc -c test/matrix.c

clean :
		rm matrix.o queue.o dthread.o matrix_test