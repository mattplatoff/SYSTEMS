#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static char myblock[5000];

typedef struct metadata{
 size_t size;
 meta* next;
 int free;
} meta; 

void* malloc(size_t size){
	int index;
	for (index = 0;index<5000;index++){
		//if block is a meta data block ( first non null in sequence ) then check size of block 
		if (myblock[index]!='\0'){
			index+=(((meta)(*myblock[index]))->size)-1;
			continue;
		}
		else
		{
			int i = 0;
			while(myblock[i] == '\0' && i < size + sizeof(meta))
			{
				i++;
			}
			if(i < size+sizeof(meta)) 
			{
				index += i;
				continue;
			}
			meta *newMeta;
			newMeta -> size = size + sizeof(meta);
			myblock[index] =  newMeta;
		}
	}
}

void free(void* mem){

}

int main(int argc, char **argv){
myblock=malloc(sizeof(char)*5000);
}