#include<master.h>

extern file* root;

char pipeBuf[PIPE_SIZE];
uint64_t pipePtr, basePtr;
bool pipeOpen = false;

DIR* sys_opendir(uint64_t* entry, uint64_t* directory){
	DIR* dir = (DIR *)directory;
	file *node = root;
	int i;
	char *path = kstrtok((char*)entry, "/");
	while(path != NULL){
		for(i=2;i<node->last;i++){
			if(kstrcmp(path, node->child[i]->name) == 0){
				node = node->child[i];
				break;
			}
		}
		if(i == node->last) return NULL;
		path = kstrtok(NULL, "/");
	}
	if(node == NULL) return NULL;
	dir->curr = 2; 
	dir->filenode = node; 
	return dir;
}

struct dirent* sys_readdir(uint64_t* entry){
	DIR *dir = (DIR*)entry;
	if (dir->filenode->last >= 3 && dir->curr< dir->filenode->last && dir->curr > 0) { 
		kstrcpy(dir->curr_dirent.name, dir->filenode->child[dir->curr]->name);
		dir->curr++;
		return &dir->curr_dirent;
	} 
	return NULL;
}

int sys_closedir(uint64_t* entry){
	DIR *dir = (DIR *)entry; 
	dir->filenode = NULL; 
	dir->curr = 0;
	return 0;
}

int sys_open(char* filename, uint64_t flags){
	FD* fd = (FD *)kmalloc(sizeof(FD));
	file *node = root;
	int i;
	char buf[250];
	kstrcpy(buf, filename);
	char *path = kstrtok(buf, "/");
	while(path != NULL){
		for(i=2;i<node->last;i++){
			if(kstrcmp(path, node->child[i]->name) == 0){
				node = node->child[i];
				break;
			}
		}
		path = kstrtok(NULL, "/");
	}
	if(node == NULL) return -1;
	i=0;
	while(currentTask->fd[i++]!=NULL);
	fd->node = node;
	fd->current = node->first;
	fd->permission = flags;
	currentTask->fd[i] = (uint64_t*)fd; 
	return i;
}

void sys_close(int fd){
	if(((FD*)currentTask->fd[fd])->node->type == PIPE_WRITE)
		pipeOpen = false;
	currentTask->fd[fd] = NULL;
}

int sys_read(int fd, uint64_t addr, uint64_t length){
	if(!fd) return gets(addr);
	FD *fdes = (FD*)currentTask->fd[fd];
	if(fdes==NULL) return 0;
	if(fdes->node->type == PIPE_READ){
		uint64_t start = fdes->current;
		while(pipePtr<=start){
			if(!pipeOpen && start>=pipePtr && start!= (uint64_t)pipeBuf){
				return -1; //pipe has been closed
			}
		 	currentTask->sleep = 1;
			currentTask->state = SLEEP;
			__asm__ __volatile__("int $32;");
		}
		if ((pipePtr - start) < length) length = pipePtr - start;
		memcpy((void *)addr, (void *)start, length);
		fdes->current += length;
		return length;
	}else{
		uint64_t end = fdes->node->last, start = fdes->current;
		if ((end - start) < length) length = end - start;
		memcpy((void *)addr, (void *)start, length);
		fdes->current += length;
		return length;
	}
}

int sys_write(int fd, uint64_t addr, int length){
	if(fd==1) return puts((char*)addr);
	FD *fdes = (FD*)currentTask->fd[fd];
	if(fdes==NULL) return 0;
	if(fdes->node->type == PIPE_WRITE){
		uint64_t end = fdes->node->last, start = fdes->current;
		if ((end - start) < length) length = end - start;
		memcpy((void *)fdes->current, (void *)addr, length);
		fdes->current += length;
		pipePtr = fdes->current;
		pipeOpen = true;
	}		
	return length;
}

int sys_lseek(int fd, int offset, int whence){
	FD *fdes = (FD*)currentTask->fd[fd];
	if(fdes ==  NULL) return 0; 
	if(whence == SEEK_SET){
		fdes->current= fdes->node->first;
		fdes->current+= offset;
	}else if(whence == SEEK_CUR){
		fdes->current+= offset;
	}else{
		fdes->current = fdes->node->last;
		fdes->current+= offset;
	}
	return offset;
}

char* sys_getcwd(char *buf, int size){
	kstrcpy(buf,currentTask->cwd);
	return buf;
}

int sys_chdir(const char *path){
	kstrcpy(currentTask->cwd, path);
	return 0;

}

pid_t sys_fork(){
	task* child = copyCurrentTaskContent(); 
	child->stack[KERNEL_STACK_SIZE-2] = currentTask->stack[KERNEL_STACK_SIZE-3];
	scheduleProcess(child);
	child->stack[KERNEL_STACK_SIZE-7] = 0UL;
	return child->pid;
}

int sys_killProcess(pid_t pid){
	task *t = currentTask, *prev = currentTask;
	while(t){
		if(t->pid == pid){
			prev->next = t->next;
			return 0;
		}
		prev = t;
		t=t->next;
	}
	kprintf("Invalid PID\n");
	return -1;	
}

uint64_t sys_execvpe(char *file, char *argv[], char *envp[]){
	task *new = createNewProc(file, argv);
	if (new){
		task *cur = currentTask;
		new->pid  = cur->pid;
		new->ppid = cur->ppid;
		kstrcpy(new->cwd, cur->cwd);
		memcpy((void*)new->fd, (void*)cur->fd, 20*8);
		cur->state = EXIT;
		__asm__ __volatile__ ("int $32");
	}
	return -1;
}

uint64_t sys_waitpid(uint64_t pid, uint64_t status, uint64_t options){
	task *t = currentTask;
	t->childWaitPid = pid;
	t->state = WAIT;
	__asm__ __volatile__ ("int $32");
	return pid;
}

extern void freePages();

void sys_exit(){
	task *t = currentTask, *parent;
	currentTask->state = EXIT;
	while(t){
		if(currentTask->ppid==t->pid) break;
		t=t->next;
	}
	if(!t) return;
	parent = t;

	t = currentTask;
	if (parent->state == WAIT && parent->childWaitPid == t->pid) {
		parent->state = READY;
	}
	freePages();
	 __asm__ __volatile__ (
        "movq %0, %%cr3;" 
        :
        : "r"(kernelPml)
    );
	__asm__ __volatile__ ("int $32");
}

int sys_sleep(int sec){
	task *t = currentTask;
	if(sec<0){
		return 0;
	} 
	t->sleep = sec;
	t->state = SLEEP;
	__asm__ __volatile__("int $32;");

	return sec;
}

uint64_t sys_brk(uint64_t pages){
	uint64_t new = currentTask->mm->brk;
	vma *p = currentTask->mm->vmaList;
	int size = PAGESIZE * pages;
	while(p){
		if (p->type == HEAP) {
			p->end += size;
			currentTask->mm->brk += size; 
			break;
		}
		p = p->next;
	}
	return new;
}

pid_t sys_getpid(){
	return currentTask->pid;
}

pid_t sys_getppid(){
	return currentTask->ppid;
}

void sys_clear(){
	clear_screen();
}

void sys_listprocess(){
	task *cur = currentTask;
	kprintf("PID\tPPID\tName\n");
	while (cur) {
		if(cur->state != EXIT)
			kprintf("%d\t%d\t%s\n", cur->pid, cur->ppid, cur->name);
		cur = cur->next;
	}
}   

void sys_signal(int signum, uint64_t handler){
	currentTask->signalNum = signum;
	currentTask->signalHandler = handler;
}

int sys_pipe(int pipefd[2]){
	memset(pipeBuf, 0, PIPE_SIZE);
	pipePtr = (uint64_t)pipeBuf;
	pipeOpen = true;
	int i=0;
	FD* fd = (FD *)kmalloc(sizeof(FD));
	while(currentTask->fd[i++]!=NULL);
	file *node = (file*)kmalloc(sizeof(file));
	node->type = PIPE_READ;
	node->first = (uint64_t)pipeBuf;
	node->current = node->first;
	node->last = (uint64_t)(pipeBuf+PIPE_SIZE);
	fd->node = node;
	fd->current = node->first;
	currentTask->fd[i] = (uint64_t*)fd;
	pipefd[0] = i++;

	fd = (FD *)kmalloc(sizeof(FD));
	node = (file*)kmalloc(sizeof(file));
	node->type = PIPE_WRITE;
	node->first = (uint64_t)pipeBuf;
	node->current = node->first;
	node->last = PIPE_SIZE;
	fd->node = node;
	fd->current = node->first;
	currentTask->fd[i] = (uint64_t*)fd;
	pipefd[1] = i;
	return 0;
} 

void* syscall_tbl[NUM_SYSCALLS] = 
{
	sys_read,
	sys_write,
	sys_brk,
	sys_fork,
	sys_execvpe,
	sys_waitpid,
	sys_exit,
	sys_getpid,
	sys_getppid,
	sys_listprocess,
	sys_opendir,
	sys_readdir,
	sys_closedir, 
	sys_open,
	sys_close,
	sys_sleep,
	sys_clear,
	sys_lseek,
	sys_killProcess,
	sys_getcwd,
	sys_chdir,
	sys_signal,
	sys_pipe
};

void syscall_handler(void)
{
								/*
											uint64_t syscallNo;
											uint64_t arg1,arg2,arg3;
											__asm volatile("movq %%rax, %0;"
											"movq %%rbx, %1;"
											"movq %%rcx, %2;"
											"movq %%rdx, %3;"
											: "=g"(syscallNo),"=g"(arg1), "=g"(arg2), "=g"(arg3)
											:
											:"rax","rsi","rcx"
											);  
											uint64_t val; 
											switch(syscallNo){
											case __NR_read:
											sys_read(arg1, arg2,arg3);
											break;
											case __NR_write:
											sys_write(arg1, arg2,arg3);
											break;
											case __NR_brk:
											sys_brk(arg1);
											break;
											case __NR_fork:
											val= sys_fork();
											__asm__ __volatile__ (
											"movq %0, %%rax;" //load next's stack in rsp
											:   
											:"r"(val)
											);  
											break;
											case __NR_execvpe:
											sys_execvpe((char*)arg1,(char**)arg2,(char**)arg3);
											break;
											case __NR_wait:
											sys_wait(arg1);
											break;
											case __NR_waitpid:
											sys_waitpid(arg1,arg2,arg3);
											break;
											case __NR_exit:
											case __NR_mmap:
											case __NR_getpid:
											case __NR_getppid:
											case __NR_listprocess:
											case __NR_opendir:
											sys_opendir((uint64_t*)arg1,(uint64_t*)arg2);
											break;
											case __NR_readdir:
											sys_readdir((uint64_t*)arg1);
											break;
											case __NR_closedir:
											sys_closedir((uint64_t*)arg1);
											break;
											case __NR_open:
											sys_open((char*)arg1,arg2);
											break;
											case __NR_close:
											sys_close(arg1);
											break;
											case __NR_sleep:
											case __NR_clear:
											case __NR_seek:
											case __NR_kill:
											case __NR_getcwd:
											case __NR_chdir:
											default:
											break;
											}
									*/
	uint64_t syscallNo;
	__asm__ __volatile__("movq %%rax, %0;" : "=r"(syscallNo));
	if (syscallNo < NUM_SYSCALLS) {
		uint64_t ret;
		__asm__ __volatile__("pushq %rdx;");
		__asm__ __volatile__(
			"movq %%rax, %0;"
			"popq %%rdx;"
			"callq *%%rax;"
			: "=a" (ret) : "r" (syscall_tbl[syscallNo])
		);
	}
	__asm__ __volatile__("iretq;");

}

