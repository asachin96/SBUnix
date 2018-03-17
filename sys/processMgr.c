#include <master.h>

task* taskList = NULL, *currentTask = NULL, *idleTask = NULL;

bool IsInitSchedule;
int nextAvailablePid = 0, tick = 0;

task* createNewTask(bool IsUserProcess, int state, char *name, char *cwd, uint64_t startPoint){
	task* new = (task*) kmalloc(sizeof(task));
	kstrcpy(new->name, name);
	new->mm = (mmStruct *) kmalloc(sizeof(mmStruct));
	new->mm->pml4 = newPml();
	kstrcpy(new->cwd, cwd);
	new->pid = nextAvailablePid++;
	new->IsUserProcess = IsUserProcess;
	new->state = state;
	new->stack[507] = startPoint;
	new->signalNum = 0;
	new->signalHandler = 0;
	int t1 = 0x23, t2 = 0x1b;
	if (!IsUserProcess) {
		t1 = 0x10;
		t2 = 0x08;
	}
	new->stack[511] = t1;
	new->stack[508] = t2; 
	new->stack[509] = 0x200202UL;
	new->stack[491] = (uint64_t)irq0 + 0x20;
	new->rsp = (uint64_t)&new->stack[490];
	return new;
}

void scheduleProcess(task* t){
	if (t->state < IDLE){
		if (t->state == RUNNING){
			t->state = READY;
		}
		task* p = taskList;
		if (p == NULL) taskList = t;
		else{
			while (p->next != NULL) p = p->next;
			p->next = t;
			t->next = NULL;
		}
	}
}

void jumpToUserMode(){
	__asm__ __volatile__(
		"movq $0x23, %rax;"
		"movq %rax,  %ds;"
		"movq %rax,  %es;"
		"movq %rax,  %fs;"
		"movq %rax,  %gs;"
	);
}

task* copyCurrentTaskContent(){
	task* t  = createNewTask(1, READY, currentTask->name, currentTask->cwd, currentTask->stack[KERNEL_STACK_SIZE-6]);
	vma *pvma = currentTask->mm->vmaList, *cvma  = NULL;
	t->ppid   = currentTask->pid;
	uint64_t ppml4   = currentTask->mm->pml4 ,cpml4 = t->mm->pml4;
	memcpy(&(t->fd[0]),&(currentTask->fd[0]),(sizeof(currentTask->fd[0])* 20));
	for(;pvma;pvma=pvma->next){
		uint64_t vdr; 
		vma *p = (vma*)kmalloc(sizeof(vma)); 
		memcpy(p, pvma,sizeof(vma));
		if(cvma == NULL){
			t->mm->vmaList = p; 
		}else{
			p->next = cvma;
		}
		cvma = p;

		if (pvma->type == STACK) {
			uint64_t buf[100];
			int j=0;
			vdr = pvma->end - STACKOFFSET;
			for(;vdr >= pvma->start;vdr -= STACKOFFSET) {
				__asm__ __volatile__ (
					"movq %0, %%cr3;"
					:
					: "r"(ppml4));
				uint64_t t = getFrame();
				virtToPhyMapper(nextAddr, t, PAGE_URW);
				buf[j++] = t;
				memcpy((void*)nextAddr, (void*)vdr, STACKOFFSET);
				unmapPteEntry(nextAddr);
			}
			vdr = pvma->end - STACKOFFSET;
			for(int i=0;i<j;i++,vdr -= STACKOFFSET){
				__asm__ __volatile__ (
					"movq %0, %%cr3;"
					:
					: "r"(cpml4));
				virtToPhyMapper(vdr, buf[i], PAGE_URW);
			}

		} else {
			vdr = pvma->start;
			for(;vdr < pvma->end;vdr += STACKOFFSET) {
				__asm__ __volatile__ (
					"movq %0, %%cr3;"
					:
					: "r"(ppml4));
				if(vdr){
					uint64_t *t = getEntry(vdr, PTE);
					*t |= PAGE_COW;
					__asm__ __volatile__ (
						"movq %0, %%cr3;"
						:
						: "r"(cpml4));
					virtToPhyMapper(vdr, getAddr(*t), getFlags(*t));
				}else{
					__asm__ __volatile__ (
						"movq %0, %%cr3;"
						:
						: "r"(cpml4));
				}
			}
		}
		__asm__ __volatile__ (
			"movq %0, %%cr3;"
			:: "r"(ppml4));	
	}
	return t;
}

task* getNextTask(){
	task *p  = taskList, *prev = NULL;
	while(p){
		if(p->state == READY){
			currentTask = p;
			if(prev!=NULL){ 
				prev->next = p->next;
				p->next  = taskList;
			}else{
				taskList = taskList->next;
			}
			return currentTask;
		}
		prev = p;
		p = p->next;
	}
	currentTask = idleTask;
	return currentTask;
}

extern void set_tss_rsp();

void timerHandler(){
	task *temp = NULL;
	tick++;
	if(tick%100 == 0){ 
		displayTime();
		task *p = taskList;
		while(p){
			if(p->state == SLEEP){
				if(p->sleep <= 0) p->state = READY;
				else p->sleep--;
			}
			p=p->next;
		}
	}
	if (IsInitSchedule) {
		if (currentTask) {
			uint64_t cur_rsp;
			__asm__ __volatile__(
				"movq %%rsp, %[cur_rsp]"
				: 
				[cur_rsp] "=r"(cur_rsp)
			);
			temp = currentTask;
			temp->rsp = cur_rsp;
			scheduleProcess(temp);
		}
		temp  = getNextTask();
		 __asm__ __volatile__ (
		 	"movq %0, %%cr3;" 
		 	:: 
		 	"r"(temp->mm->pml4)
		 );
		__asm__ __volatile__(
			"movq %[next_rsp], %%rsp" 
			:: 
			[next_rsp] "m" (temp->rsp)
		);
		if (temp->IsUserProcess) {
			set_tss_rsp((uint64_t)&temp->stack[KERNEL_STACK_SIZE-1]);
			jumpToUserMode();
		}
	}
	outb(0x20,0x20);
}
