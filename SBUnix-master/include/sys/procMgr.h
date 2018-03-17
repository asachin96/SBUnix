#ifndef _PROCESSMGR_H
#define _PROCESSMGR_H
struct vma;
typedef struct mmStruct{
    uint64_t pml4, brk, stack;
    struct vma *vmaList;           
}mmStruct;

typedef struct vma{
    uint64_t start, end, type;               
    mmStruct *mm;          
    struct vma *next;         
}vma;

typedef struct task{
    pid_t pid, ppid, childWaitPid;
    bool IsUserProcess;
    uint64_t stack[512], rsp, state, sleep, *fd[20];            
    mmStruct* mm; 
    char name[30], cwd[250];                             
    struct task* next;              
    int signalNum;
    uint64_t signalHandler;
}task;

void scheduleProcess(task*);
void kmmap(uint64_t, int, uint64_t);
task* createNewTask(bool, int, char*, char*, uint64_t);
task* copyCurrentTaskContent();

//task states
#define RUNNING 1
#define READY 2
#define WAIT 3
#define SLEEP 4
#define IDLE 5 
#define EXIT 6

//vma type
#define TEXT 1
#define DATA 2
#define HEAP 3
#define STACK 4

#endif
