#include <master.h>

#define INITIAL_STACK_SIZE 4096
#include <sys/idt.h>
#include <sys/timer.h>
#define K_MEM_PAGES 518
uint8_t initial_stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
void *physfree_global;
extern bool IsInitSchedule;
extern void initTarfs();

void idleFunc(){
	while(11);
}

void start(uint32_t *modulep, void *physbase, void *physfree){
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap);

		physfree_global = physfree;
	initFrame((uint64_t)0x400000, (uint64_t) (physfree + KERNEL_START_VADDR)); 
	setupPageTable((uint64_t)&kernmem, (uint64_t)physbase, K_MEM_PAGES);
	__asm__ __volatile__("movq %0, %%rbp" : :"a"(&initial_stack[0]));
	__asm__ __volatile__("movq %0, %%rsp" : :"a"(&initial_stack[INITIAL_STACK_SIZE]));
	initTarfs();
	idleTask = createNewTask(0, IDLE, "idle", "/", (uint64_t)idleFunc);
	idleTask->stack[KERNEL_STACK_SIZE-2] =  (uint64_t)&idleTask->stack[KERNEL_STACK_SIZE-1];
	scheduleProcess(idleTask);
	createNewProc("/rootfs/bin/init",NULL);
	IsInitSchedule = 0;
	__asm__("sti;");
	IsInitSchedule = 1;
	while(1); //main should not return
}

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used

	__asm__ volatile (
																	//																			"cli;"
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&initial_stack[INITIAL_STACK_SIZE])
		);

	__asm__ volatile("cli");
	init_gdt();
	installIdt();
	initTimer();
	enable_cursor(0,15);	
	clear_screen(); 
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
		);
	while(1) __asm__ volatile ("hlt");
}
