#include <syscall.h>
#include <sys/defs.h>
#include <stdlib.h>
#include <unistd.h>

#define listSize 1024

struct mallocManager{
    int size;
    void *ptr;
    int isFree;
};

static struct mallocManager tracker[listSize];
static int init = 1;
void* malloc(int size)
{
    int i=0;
    if(init){
        for(;i<listSize;i++){
           tracker[i].size=0;
           tracker[i].ptr=NULL;
           tracker[i].isFree=1;
        }
        init =0;
    } 
    for(i=0;i<listSize;i++){
        if(tracker[i].isFree == 1&&tracker[i].size>=size){
            tracker[i].isFree = 0;
            return tracker[i].ptr;
        }
    }
    
    char* retVal = brk(size);
    for(i=0;i<listSize;i++){
       if(tracker[i].isFree==1){
           tracker[i].isFree = 0;
           tracker[i].ptr = retVal;
           break;
       }
    }
    return retVal;
}

void free(void *p)
{
    int i=0; 
    for(i=0;i<listSize;i++){
        if(tracker[i].ptr == p){
            tracker[i].isFree = 1;
            break;
        }
    }
    return;
}

