#include <master.h>

void* lookup(char *filename) {
	uint64_t ptr = (uint64_t) &_binary_tarfs_start;
	int size = 0; 
	while(ptr < (uint64_t)&_binary_tarfs_end){
		tarfsHeader *header = (tarfsHeader*)ptr;
		size = toDecimal(atoi(header->size));
		if (kstrcmp(filename, header->name) == 0) {   
			return (void*)(header+1);
		}
		ptr += sizeof(tarfsHeader);
		if (size > 0) {
			ptr += size + sizeof(tarfsHeader) - size % sizeof(tarfsHeader);
		}
	}
	return NULL;
}

void loadElf(ElfHdr* header, task *proc, char *filename, char *argv[]){
	ElfPHdr* program_header;
	mmStruct *mms = proc->mm;
	vma *node = NULL, *last = NULL;
	uint64_t pml4;

	 __asm__ __volatile__ (
	 	"movq %%cr3, %0;" 
	 	: 
	 	"=r"(pml4)
	 );
	kstrcpy(proc->name, filename);
	program_header = (ElfPHdr*) ((void*)header + header->e_phoff);
	int i=0;
	while (i < header->e_phnum) {
		if ((int)program_header->p_type == 1) {     
			node = (vma*) kmalloc(sizeof(vma));
			node->start = program_header->p_vaddr;
			node->end = program_header->p_vaddr +  program_header->p_memsz;
			__asm__ __volatile__ (
				"movq %0, %%cr3;" 
				:: 
				"r"(mms->pml4)
			);
			uint64_t start = node->start;
			if(program_header->p_flags == 5){
				node->type = TEXT;
				allocatePages(&start, PAGE_UR, getNoPages(node->start, program_header->p_memsz));
			}else{
				allocatePages(&start, PAGE_URW, getNoPages(node->start, program_header->p_memsz));
				node->type = DATA;
			}

			if(last) last->next = node;
			else mms->vmaList = last = node;
			last = node;

			memcpy((void*) node->start, (void*) ((uint64_t)header + program_header->p_offset), program_header->p_filesz);
			__asm__ __volatile__ (
				"movq %0, %%cr3;" 
				:: 
				"r"(pml4)
			);
		}
		program_header++;
		i++;
	}

	node  =  (vma*) kmalloc(sizeof(vma));
	node->type = HEAP;
	last = last->next = node;

	node = (vma*) kmalloc(sizeof(vma));
	node->start = 0xEFFFFF0000UL;
	node->end = 0xF000000000UL;
	node->type = STACK;
	last->next = node;
	 __asm__ __volatile__ (
	 	"movq %0, %%cr3;" 
	 	:: 
	 	"r"(mms->pml4)
	 );
	uint64_t start = 0xEFFFFFF000UL;
	allocatePages(&start, PAGE_URW, PAGESIZE);
	 __asm__ __volatile__ (
	 	"movq %0, %%cr3;" 
	 	:: 
	 	"r"(pml4)
	 );

	int argc = 1;
	char arguments[5][30];
	kstrcpy(arguments[0],proc->name);
	if(argv){
		while(argv[argc-1]!=NULL){
			kstrcpy(arguments[argc], argv[argc-1]);
			argc++;
		} 
	}

 	__asm__ __volatile__ (
 		"movq %0, %%cr3;" 
 		::
 		 "r"(proc->mm->pml4)
 	);

	uint64_t *stack = (uint64_t*) 0xEFFFFFFFF8;
	i=argc-1;
	uint64_t *temp[5];
	while(i>=0){
		stack = (uint64_t*)((char*)stack - kstrlen(arguments[i]));
		memcpy((char*)stack, arguments[i], kstrlen(arguments[i]));
		temp[i--] = stack;
	}
	i=argc-1;
	while(i>=0)	*(--stack) = (uint64_t)temp[i--];
	*(--stack) = (uint64_t)argc;

	proc->mm->stack = (uint64_t)stack;

	__asm__ __volatile__ (
		"movq %0, %%cr3;" 
		:: 
		"r"(pml4)
	);
}

task* createNewProc(char *filename, char *argv[]){
	int i;
	char pathname[250];
	memset(pathname, 0,  250);
	kstrcpy(pathname, filename);
	char *path = kstrtok(pathname, "/");  
	file *node = root;
	while(path != NULL){
		for(i=2;i<node->last;i++){
			if(kstrcmp(path, node->child[i]->name)==0){
				node = node->child[i];
				break;
			}
		}
		if(i == node->last) return NULL;
		path = kstrtok(NULL, "/");
	}  
	ElfHdr* header = (ElfHdr *)node->first;
	task* proc = createNewTask(1, READY, filename, currentTask? currentTask->cwd: "/", header->e_entry);
	loadElf(header, proc, filename, argv);
	proc->stack[KERNEL_STACK_SIZE-2] = proc->mm->stack;
	scheduleProcess(proc);
	return proc;
}

