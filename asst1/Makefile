CC = gcc
CCFLAGS = -Wall

all: 
	memgrind

memgrind: memgrind.c mymalloc.c mymalloc.h
	$(CC) $(CCFLAGS) -o memgrind memgrind.c

clean:
	rm mymalloc memgrind		