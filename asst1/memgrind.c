#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"
#include "mymalloc.c"

/*TO-DO:
- make two original test cases
*/

int testA()
{
	
	struct timeval start, end;
	int totalTime = 0;
	int i;
	for(i = 0; i < 100; i++)
	{
		gettimeofday(&start,NULL);
		char* arr[1000];
		int x;
		for(x = 0; x < 1000; x++)
		{
			arr[x] = (char*)malloc(1);
		}

		for(x = 0; x < 1000; x++)
		{
			free(arr[x]);
		}
		gettimeofday(&end,NULL);
		totalTime += ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	}

	return totalTime/100;
}

int testB()
{
	struct timeval start, end;
	int totalTime = 0;
	int i;
	for(i = 0; i < 100; i++)
	{
		gettimeofday(&start,NULL);
		int x;
		for(x = 0; x < 1000; x++)
		{
			char* ptr = (char*)malloc(1);
			free(ptr);
		}
		gettimeofday(&end,NULL);
		totalTime += ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	}

	return totalTime/100;
}

int testC()
{
	struct timeval start, end;
	int totalTime = 0;
	int i;
	for(i = 0; i < 100; i++)
	{
		gettimeofday(&start,NULL);
		srand(time(NULL));
		char* arr[1000];
		int currIndex = 0;
		int mcount = 0;

		while(mcount < 1000)
		{
			int r = rand() % 2;
			if(r == 0)
			{
				arr[currIndex] = (char*)malloc(1);
				currIndex++;
				mcount++;
			}
			else if(r == 1 && currIndex > 0)
			{
				currIndex--;
				free(arr[currIndex]);
			}
		}

		while(currIndex > 0)
		{
			currIndex--;
			free(arr[currIndex]);
		}
		gettimeofday(&end,NULL);
		totalTime += ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	}
	return totalTime/100;
}

int testD()
{
	struct timeval start, end;
	int totalTime = 0;
	int i;
	for(i = 0;i<100;i++)
	{
		gettimeofday(&start,NULL);
		srand(time(NULL));
		char* arr[1000];
		int mallocsize[1000];
		int currSize = MEM_SIZE;
		int currIndex = 0;
		int mcount = 0;

		while(mcount < 1000)
		{
			int r = rand() % 2;
			int size = (rand() % 64) + 1;
			if(r == 0 && (size + sizeof(meta)) <= currSize)
			{
				arr[currIndex] = (char*)malloc(size);
				mallocsize[currIndex] = size + sizeof(meta);
				currSize -= size + sizeof(meta);
				currIndex++;
				mcount++;
			}
			else if(r == 1 && currIndex > 0)
			{
				currIndex--;
				free(arr[currIndex]);
				currSize += mallocsize[currIndex];
			}
		}

		while(currIndex > 0)
		{
			currIndex--;
			free(arr[currIndex]);
		}
		gettimeofday(&end,NULL);
		totalTime += ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	}
		return totalTime/100;	
}

int main()
{
	printf("Test A's average run time was: %d microseconds.\n",testA());
	printf("Test B's average run time was: %d microseconds.\n",testB());
	printf("Test C's average run time was: %d microseconds.\n",testC());
	printf("Test D's average run time was: %d microseconds.\n",testD());
}