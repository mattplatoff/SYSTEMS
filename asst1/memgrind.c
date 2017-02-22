#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"
#include "mymalloc.c"

/*TO-DO:
- test A-D
- do timing for all test cases
- do each test case 100 times
- calculate average time for each one over sample of 100 times
- handle error/otherwise special cases in A-D
- make two original test cases
*/

int testA()
{
	struct timeval start, end;
	long totalTime = 0;
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
		totalTime += (end.tv_sec - start.tv_sec)*1000000L+ (end.tv_usec - start.tv_usec);
	}

	return totalTime/100;
}

int testB()
{
	int x;
	for(x = 0; x < 1000; x++)
	{
		char* ptr = (char*)malloc(1);
		free(ptr);
	}

	return 0;
}

int testC()
{
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

	return 0;
}

int testD()
{
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

	return 0;	
}

int main()
{
	//printf("%d\n",MEM_SIZE);
	printf("%d\n",testA());
}