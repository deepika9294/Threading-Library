matrix_test : dthread.o singlyll.o matrix.o
			cc -o matrix_test dthread.o singlyll.o matrix.o

dthread.o : dthread.c dthread.h singlyll.h
			cc -c dthread.c

singlyll.o : singlyll.c singlyll.h dthread.h
			cc -c singlyll.c

matrix.o : test/matrix.c dthread.h
			cc -c test/matrix.c

clean :
		rm matrix.o singlyll.o dthread.o matrix_test