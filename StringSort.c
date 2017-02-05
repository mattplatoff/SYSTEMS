#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define LC(x) 2 * x + 1
#define RC(x) 2 * x + 2
#define PAR(x) (x - 1) / 2

//TODO: make all methods return something, at least an int to indicate status
//TODO: clean up code and comment for submission
//TODO: test and generate edge cases

typedef struct HeapNode
{
	char * word;
} HeapNode;

typedef struct LexHeap
{
	int size;
	HeapNode * root;
} LexHeap;

LexHeap buildHeap(){
	LexHeap heap;
	heap.size=0;
	return heap;
}

void push(LexHeap *heap, char* word) {
	if(heap->size) 
	{
        heap->root = realloc(heap->root, (heap->size + 1) * sizeof(HeapNode));
    } 
    else {
        heap->root = (HeapNode*)malloc(sizeof(HeapNode)) ;
    }
    // initializing the node with value
    HeapNode node;
    node.word = word ;

    // Positioning the node at the right position in the min heap
    int i = (heap->size)++ ;
    while(i && strcmp(node.word, heap->root[PAR(i)].word)<0) 
    {
        heap->root[i] = heap->root[PAR(i)];
        i = PAR(i);
    }
    
    heap->root[i] = node;
}

void swap(HeapNode *n1, HeapNode *n2) {
    HeapNode temp = *n1 ;
    *n1 = *n2 ;
    *n2 = temp ;
}

void heapify(LexHeap *heap, int i) {
    int smallest = (LC(i) < heap->size && strcmp(heap->root[LC(i)].word, heap->root[i].word)<0) ? LC(i) : i ;
    if(RC(i) < heap->size && strcmp(heap->root[RC(i)].word, heap->root[smallest].word) < 0) {
        smallest = RC(i) ;
    }
    if(smallest != i) {
        swap(&(heap->root[i]), &(heap->root[smallest])) ;
        heapify(heap, smallest) ;
    }
}

char* pop(LexHeap *heap) {
    if(heap->size) {
        char* ret = (heap->root)->word;
        heap->root[0] = heap->root[--(heap->size)] ;
        heap->root = realloc(heap->root, heap->size * sizeof(HeapNode)) ;
        heapify(heap, 0) ;
        return ret;
    } else {
        free(heap->root) ;
        fprintf(stderr, "ERROR: Heap is empty, element could not be removed \n");
        return NULL;
    }
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "ERROR: Invalid number of arguments.\nUsage: ./StringSort <Input String>\n");
        return 1;
    }
    LexHeap heap = buildHeap();
	char* inputString = argv[1];
	int len = strlen(inputString);
    int tokenLength = 0; //will keep track of length of each token for mallocing
	char* ptr;

    int index;
    for (index = 0; index<= len;index++)
    {
        if (!isalpha(inputString[index]) || index == len)
        {
           if(tokenLength == 0) {continue;}
            //allocate memory for token
            ptr = (char*)malloc(sizeof(char)*tokenLength+1);
            if(ptr == NULL)
            {
                fprintf(stderr, "Malloc failed to allocate on line 104...exiting\n");
                return 1;
            }
            //copy vals from input string to token string and add null terminating byte.
            memcpy(ptr,&inputString[index-tokenLength],tokenLength);
            ptr[tokenLength]='\0';
            //printf("%s\n", ptr);
            push(&heap,ptr);
            tokenLength=0;
            free(ptr);
        }
        else tokenLength++;        
    }

    while(heap.size>0){
        char * ret = pop(&heap);
        printf("%s\n", ret);
    }
    return 0;
}


