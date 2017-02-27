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
			if(r == 0 && arr[currIndex] != NULL)
			{
				arr[currIndex] = (char*)malloc(1);
				if(arr[currIndex] != NULL)
				{
					currIndex++;
					mcount++;
				}
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

int testE()
{
	struct timeval start, end;
	int totalTime = 0;
	int x;
	for(x = 0;x<100;x++)
	{
		gettimeofday(&start, NULL);
		char* arr[MEM_SIZE/(2 + sizeof(meta))];
		int i = 0;
		int j;
		for(; i < 1000; i++)
		{
			arr[i] = (char*)malloc(2);
		}

		for(j = 0; j < i; j+= 2)
		{
			free(arr[j]);
		}

		for(j = 0; j < i; j+= 2)
		{
			arr[j] = (char*)malloc(1);
		}

		for(j = 0; j < i; j++)
		{
			free(arr[j]);
		}

		gettimeofday(&end, NULL);
		totalTime += ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	}

	return totalTime/100;
}

int testF(){
	struct timeval start, end;
	int totalTime = 0;
	int x;
	char* arr[MEM_SIZE/(2 + sizeof(meta))];
	gettimeofday(&start, NULL);
	for (x=0;x<1000;x++){
		arr[x++]=(char*)malloc(1);
		arr[x++]=(char*)malloc(1);
		arr[x]=(char*)malloc(1);
		free(arr[x-1]);
	}

	while(arr[x] != NULL)
	{
		free(arr[x]);
	}

	gettimeofday(&end, NULL);
	totalTime += ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	

	return totalTime;
}

int main(int argc, char**argv)
{
	if(argc != 1)
	{
		fprintf(stderr, "ERROR: INVALID NUMBER OF ARGUMENTS\n");
		return 1;
	}
	printf("Test A's average run time was: %d microseconds.\n",testA());
	printf("Test B's average run time was: %d microseconds.\n",testB());
	printf("Test C's average run time was: %d microseconds.\n",testC());
	printf("Test D's average run time was: %d microseconds.\n",testD());
	printf("Test E's average run time was: %d microseconds.\n",testE());
	printf("Test F's average run time was: %d microseconds.\n",testF());

	return 0;
}