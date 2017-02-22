CC = gcc
CCFLAGS = -Wall

all: 
	mymalloc memgrind

mymalloc: mymalloc.c mymalloc.h
	$(CC) $(CCFLAGS) -o mymalloc mymalloc.c

memgrind: memgrind.c mymalloc.c mymalloc.h
	$(CC) $(CCFLAGS) -o memgrind memgrind.c

clean:
	rm mymalloc memgrind		