#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MEM_SIZE 5000
#define malloc(x) mymalloc(x)
#define free(x) myfree(x)

static char myblock[5000];

/*
process for malloc:
	if block is first to be malloced:
		mem requested > mem available ? err : allocate mem and return pointer to first block after metadata:

	else:
		get first meta data, check m1 size against m2 address and find how much empty space there is.
			if the second block and its metadata fits in the empty space, place it there, update next pointers of previous meta data
			else proceede to next meta data.
			if metadata.next = null compare size of that meta data with address of last block in array.  
			if there is room for requested space, return pointer to block, else err

process for free:
	TBD


*/
typedef struct metadata meta;
struct metadata{
 int size;
 meta* next;
 char free;
 //0 if used, 1 if free
}; 

void* mymalloc(int size){
	//on first malloc
	void* ret;
	if (myblock[0]=='\0'){
		meta firstMeta;
		firstMeta.size = size + sizeof(meta) >= MEM_SIZE ? fprintf(stderr,"ERROR, MEM OUT OF BOUNDS\n") : size;
		firstMeta.free = 0;
		firstMeta.next = NULL;
		//store first metadata struct in address of first block in array



		memcpy(&myblock[0],&firstMeta,sizeof(meta));


		//get address of first free block after metadata and return that as void*
		ret = (void*)(&myblock[0] + sizeof(meta));
		return ret;

	}
	else {
		//start with a pointer to first metadata
		meta* ptr = (meta*)&myblock[0];
		//size of block being checked for storage

		while(ptr!=NULL){
			//consoladate adjecent free blocks of memory
			if (ptr->free&&ptr->next&&ptr->next->free){
				ptr->size+=ptr->next->size+sizeof(meta);
				ptr->next=ptr->next->next;
			}
			//if block is already free, no need to create new metadata, just update current one and return pointer. 
			if (ptr->free&&(ptr->size<=size+sizeof(meta))){
				ptr->size=size;
				return (void*)(ptr+sizeof(meta));
				}
			meta* currentMemSpaceEnd = ptr->next? ptr->next : (meta*)&myblock[MEM_SIZE];
			
			// if block requested fits inbetween currently allocated block and next block, make new metadata and return pointer to allocated block
			if ((ptr+ptr->size+sizeof(meta))-(currentMemSpaceEnd)>(sizeof(meta)+size)){
				//insert new pointer 
				meta newMeta;
				newMeta.size=size;
				newMeta.free=0;
				newMeta.next=ptr->next;

				//place metadata in array directly after the allocated block
				memcpy(ptr + ptr->size + sizeof(meta),&newMeta,sizeof(meta));

				ptr->next=&newMeta;
				
				//return pointer to first empty byte after metadata
				return (void*)(ptr+ptr->size+(2*sizeof(meta)));
			}
			else {
				if (ptr->next){
					ptr=ptr->next;
				}
				else 
					fprintf(stderr, "ERROR: OUT OF MEMORY, FREE ALLOCATED MEMORY\n");
			}
		} 
	}
	return NULL;
}

void myfree(void* mem){

	//print error if user is attempting to free a block of memory that is outside the bounds of the simulated "memory"
	//unsure if the address of mem will actually be withing the range of 0-5000 or if it will have a legitimate address according to actual memory management
//does this work @mohit? 
	if((char*)mem > &myblock[5000])

	{
		fprintf(stderr, "ERROR: ATTEMPTING TO FREE MEMORY THAT IS OUT OF BOUNDS\n");
		return;
	}

	//CHECK IF (MEM - SIZE OF META) ACTUALLY POINTS TO A STRUCT

	//make pointer to first byte of metadata corresponding to memory to be freed
	meta* ptr = (meta*)(mem - sizeof(meta));
	
	//print error if already free memory is attempting to be be freed
	if(ptr->free)
	{
		fprintf(stderr, "ERROR: ATTEMPTING TO FREE MEMORY THAT HAS ALREADY BEEN FREED\n");
		return;
	}
	else
	{
		ptr->free = 1;
		/*maybe need to set the actual freed blocks of memory to '\0', but probably not neccessary because malloc 
			just checks if the space is free in the metadata (except for initial block) (psuedo code for process below)*/
		
		//add size of meta struct to ptr

		//loop through each byte of memory setting it to '\0'

		//stop when next metadata block is reached (unsure how to check if current memory is a metadata block)
	}
	
}

int main(int argc, char **argv){
//allocate and zero out memory CHECK IF THIS MALLOC CALL CONFLICTS WITH THE ONE I WROTE WHEN CALLED
char* test = (char*)malloc(sizeof(char)*4);
puts("test2\n");
printf("%p\t%p\t%d\n",(void*)&myblock[0],(void*)test,sizeof(meta));
test[0] = 'h';
test[1] = 'e';
test[2] = 'l';
test[3] = '\0';
printf("%p\n",(void*)test);

printf("%s\n",test);
free(test);
return 0;
}