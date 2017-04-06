# Header file containing all structures used
# @Authors: jcbaker, sccoache
# CS3013 Project 3b

all: mutex semaphore

semaphore: semaphore.o
	gcc -g semaphore.o -o semaphore -lpthread
semaphore.o: semaphore.c header.h
	gcc -g -c semaphore.c -lpthread

mutex: mutex.o
	gcc -g mutex.o -o mutex -lpthread
mutex.o: mutex.c header.h
	gcc -g -c mutex.c -lpthread

clean:
	rm -f *.o mutex semaphore
