#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MEM_SIZE 5000

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
typedef struct metadata{
 int size;
 meta* next;
 char free;
 //0 if used, 1 if free
} meta; 


void* malloc(int size){
	//on first malloc
	void* ret;
	if (myblock[0]=='\0'){
		meta firstMeta;
		firstMeta.size = size + sizeof(meta) >= MEM_SIZE ? fprintf(stderr,"ERROR, MEM OUT OF BOUNDS\n") : size;
		firstMeta.free = 0;
		firstMeta.next = NULL;
		
		//store first metadata struct in address of first block in array
		(meta*)(&myblock[0]) = firstMeta;
		//get address of first free block after metadata and return that as void*
		ret = (void*)(&myblock[sizeof(meta)]);
		return ret;

	}
	else {
		//start with a pointer to first metadata
		meta* ptr = (meta)&myblock[0];
		//size of block being checked for storage
		int blocksize = 0;
		while(ptr!=null){
			//consoladate adjecent free blocks of memory
			if (ptr->free&&ptr->next&&ptr->next->free){
				ptr->size+=ptr->next->size+sizeof(meta);
				ptr->next=ptr->next->next;
			}
			//if block is already free, no need to create new metadata, just update current one and return pointer. 
			if (ptr->free&&(ptr->size<=size+sizeof(meta))){
				ptr->size=size;
				return (void*)(&ptr+sizeof(meta));
				}
			meta* currentMemSpaceEnd = ptr->next? ptr->next : &myblock[MEM_SIZE];
			
			// if block requested fits inbetween currently allocated block and next block, make new metadata and return pointer to allocated block
			if ((&ptr+ptr->size+sizeof(meta))-&(currentMemSpaceEnd)>(sizeof(meta)+size)){
				//insert new pointer 
				meta newMeta;
				newMeta.size=size;
				newMeta.free=0;
				newMeta.next=ptr->next;
				//place metadaa in array directly after the allocated block
				(meta*)(&ptr+ptr->size+sizeof(meta)) = newMeta;
				ptr->next=&newMeta;
				
				//return pointer to first empty byte after metadata
				return (void*)(&ptr+ptr->size+(2*sizeof(meta)));
			}
			else {
				if (ptr->next){
					ptr=ptr->next;
				}
				else 
					fprintf(stderr, "ERROR: OUT OF MEMORY, FREE ALLOCATED MEMORY");
			}
		} 
	}
}

void free(void* mem){

}

int main(int argc, char **argv){
//allocate and zero out memory CHECK IF THIS MALLOC CALL CONFLICTS WITH THE ONE I WROTE WHEN CALLED
myblock=malloc(sizeof(char)*5000)
int index = 0;
for(index = 0; index <5000; index++){
	myblock[index]='\0';
} 

}