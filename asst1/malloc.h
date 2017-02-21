#ifndef MALLOC_H_
#define MALLOC_H_

#define MEM_SIZE 5000
#define malloc(x) mymalloc(x, __FILE__,__LINE__)
#define free(x) myfree(x,__FILE__,__LINE__)

void* mymalloc(size_t,char*,int);
void myfree(void*,char*,int);

#endif