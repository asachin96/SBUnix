#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/ahci.h>
//#include <sys/page.h>
#include <sys/paging.h>
#include <sys/phys_mm.h>
#include <sys/virt_mm.h>
#include <sys/kmalloc.h>
#include <sys/proc_mngr.h>
#include <sys/init_desc_table.h>

#define INITIAL_STACK_SIZE 4096
#include <sys/idt.h>
#include <sys/timer.h>
#define K_MEM_PAGES 518
//extern void installIdt();
//uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint8_t initial_stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
void *physfree_global;
extern void initSchedule();
extern void schedule(); 
extern bool IsInitSchedule;
//extern void initPic();
void fun1(void){
   int i = 0;
   while(i<4){
       kprintf("%d in func1\n", i++);
							schedule();
   }
   kprintf("out of fun1");
   while(1);
}

void fun2(void){
   int i = 0;
   while(i<4){
       kprintf("%d in func2\n", i++);
							schedule();
   }
   kprintf("out of fun2");
   while(1);
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
								uint64_t phys_size = 0;
								uint64_t phys_base = 0;

								struct smap_t {
																uint64_t base, length;
																uint32_t type;
								}__attribute__((packed)) *smap;
								while(modulep[0] != 0x9001) modulep += modulep[1]+2;
								for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
																if (smap->type == 1 /* memory */ && smap->length > 0x3000) {
																								kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
																								phys_base = smap->base;
																								phys_size = smap->length;

																}
								}
								//kprintf("physfree %p\n\r", (uint64_t)physfree);
								//kprintf("tarfs in [%p:%p] \n%d", &_binary_tarfs_start, &_binary_tarfs_end);

			 				physfree_global = physfree;
								kprintf("physbase: %p, size: %x", phys_base, phys_size);
			  			//phys_init((uint64_t)phys_base, (uint64_t) physfree, phys_size); 
					 		phys_init((uint64_t)0x100000, (uint64_t) physfree, 0x5fcb000); 
								init_paging((uint64_t)&kernmem, (uint64_t)physbase, K_MEM_PAGES);

								kprintf("After page init!!");
        // Reset the kernel stack
    __asm__ __volatile__("movq %0, %%rbp" : :"a"(&initial_stack[0]));
    __asm__ __volatile__("movq %0, %%rsp" : :"a"(&initial_stack[INITIAL_STACK_SIZE]));
							init_tarfs();
       
							create_idle_process();
       create_elf_proc("/rootfs/bin/hello",NULL);
       //create_elf_proc("/rootfs/bin/fork",NULL);
       create_elf_proc("/rootfs/bin/ps",NULL);
       // createKernelProcess((uint64_t)fun1);
        //createKernelProcess((uint64_t)fun2);
       // initSchedule();
        IsInitSchedule = FALSE;
								__asm__("sti;");
        IsInitSchedule = TRUE;

        kprintf("\nEnd of Kernel");
								while(1); //main should not return
}

void boot(void)
{
								// note: function changes rsp, local stack variables can't be practically used
								register char *temp1, *temp2;

								for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
								__asm__ volatile (
					//																			"cli;"
																								"movq %%rsp, %0;"
																								"movq %1, %%rsp;"
																								:"=g"(loader_stack)
																								:"r"(&initial_stack[INITIAL_STACK_SIZE])
																							);

        __asm__ volatile("cli");
								init_gdt();
  // init_tss();
  //  init_idt();
  // init_pic();

								installIdt();
								initTimer();
								start(
																								(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
																								&physbase,
																								(void*)(uint64_t)loader_stack[4]
													);
								for(
																								temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
																								*temp1;
																								temp1 += 1, temp2 += 2
											) *temp2 = *temp1;
								while(1) __asm__ volatile ("hlt");
}
