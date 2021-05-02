default : run

build : matrix_test spin_test methods_test search_test

run : build	
	./run.sh

matrix_test : dthread.o queue.o matrix.o
			cc -o matrix_test dthread.o queue.o matrix.o

spin_test : dthread.o queue.o spin.o
			cc -o spin_test dthread.o queue.o spin.o

methods_test : dthread.o queue.o  methods.o
			cc -o methods_test dthread.o queue.o methods.o

search_test : dthread.o queue.o search.o
			cc -o search_test dthread.o queue.o search.o

dthread.o : dthread.c dthread.h queue.h
			cc -c dthread.c

queue.o : queue.c queue.h dthread.h
			cc -c queue.c

matrix.o : test/matrix.c dthread.h
			cc -c test/matrix.c

spin.o : test/spin.c dthread.h
			cc -c test/spin.c

methods.o : test/methods.c dthread.h
			cc -c test/methods.c

search.o : test/search.c dthread.h
			cc -c test/search.c

clean :
		rm matrix.o queue.o dthread.o spin.o methods.o search.o matrix_test spin_test methods_test search_test