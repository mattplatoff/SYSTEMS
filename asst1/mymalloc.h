#ifndef MALLOC_H_
#define MALLOC_H_

#define malloc(x) mymalloc(x, __FILE__,__LINE__)
#define free(x) myfree(x,__FILE__,__LINE__)
//#define MEM_SIZE 50000
void* mymalloc(size_t,char*,int);
void myfree(void*,char*,int);

typedef struct metadata meta;
struct metadata{
 char free;
 size_t size;
 meta* next;
 //0 if used, 1 if free
}; 

#define MEM_SIZE (5000 * sizeof(meta))
#endif