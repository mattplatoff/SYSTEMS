#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define LC(x) 2 * x + 1
#define RC(x) 2 * x + 2
#define PAR(x) (x - 1) / 2

typedef struct HeapNode
{
	Char * word;
} HeapNode;

typedef struct LexHeap
{
	int size;
	HeapNode * root;
} LexHeap;

LexHeap buildHeap(int size){
	LexHeap heap;
	heap.size=0;
	return heap;
}

void push(LexHeap *heap, Char* word) {
	if(heap->size) 
	{
        heap->root = realloc(heap->root, (size + 1) * sizeof(node));
    } 
    else {
        heap->root = malloc(sizeof(node)) ;
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
    if(RC(i) < heap->size && strcmp(heap->root[RC(i)].word, heap->root[largest].word) < 0) {
        smallest = RC(i) ;
    }
    if(smallest != i) {
        swap(&(heap->root[i]), &(heap->root[smallest])) ;
        heapify(heap, smallest) ;
    }
}

HeapNode pop(LexHeap *heap) {
	if(heap->size) {
        heap->root[0] = heap->root[--(heap->size)] ;
        heap->root = realloc(heap->root, heap->size * sizeof(HeapNode)) ;
        heapify(heap, 0) ;
    } else {
        free(heap->root) ;
    }
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "ERROR: Invalid numver of arguments.\nUsage: ./StringSort <Input String>\n");
        return 1;
    }
	char* inputString = argv[1];
	int len = strlen(inputString);
	for (int index = 0; i< len;index++){
        if (!isAlpha(inputString[index])){
            //create new pointer and add line terminating character to array

            //Create new node

            //push node onto heap
        }        
    }
}


