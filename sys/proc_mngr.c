#include <sys/kmalloc.h>
#include <sys/virt_mm.h>
#include <sys/phys_mm.h>
#include <sys/proc_mngr.h>
#include <sys/kprintf.h>
//#include <screen.h>
#include <sys/paging.h>
#include <sys/util.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/timer.h>
#include <sys/kstring.h>

int tick=0;
extern void displayTime();
extern void set_tss_rsp();
extern void sys_listprocess();
#define switch_to(prev, next) \
    __asm__ __volatile__(\
        /* Push all Registers into Stack */\
        "pushq %%rax\n\t"\
        "pushq %%rbx\n\t"\
        "pushq %%rcx\n\t"\
        "pushq %%rdx\n\t"\
        "pushfq\n\t"\
        "pushq %[my_last]\n\t"\
        \
        "pushq %%rbp\n\t"\
        "movq %%rsp, %[prev_sp]\n\t"    /* Store current Stack Pointer */\
        "movq %[next_sp], %%rsp\n\t"    /* Restore Stack Pointer of next process */\
        "movq $label, %[prev_ip]\n\t"   /* Assign IP for current process to label */\
        "pushq %[next_ip]\n\t"          /* Push IP of next process to be switched */\
        "jmp read_ip\n\t"               /* Call read_ip function to read next_ip on stack */\
    "label:\t"\
        /* Pop all Registers from Stack */\
        "popq %%rbp\n\t"\
        "popq %[new_last]\n\t"\
        "popfq\n\t"\
        "popq %%rdx\n\t"\
        "popq %%rcx\n\t"\
        "popq %%rbx\n\t"\
        "popq %%rax\n\t"\
        :[prev_sp]  "=m" (prev->rsp_register),\
         [prev_ip]  "=m" (prev->rip_register),\
         [new_last] "=m" (prev)\
        :[next_sp]  "m"  (next->rsp_register),\
         [next_ip]  "m"  (next->rip_register),\
         [my_last]  "m"  (prev));\

// Task list. The next READY task on the list will be executed.
static task_struct* next_task_list = NULL;
// Current running task`
task_struct* CURRENT_TASK = NULL;
// Idle task. This will run only when there is no READY task to be run
static task_struct* idle_task = NULL;

static task_struct* prev = NULL;
static task_struct* next = NULL;

bool IsInitSchedule;

void read_ip()
{
}

// Idle kernel task
static void idle_process(void)
{
								//kprintf("\nInside Idle Process ");
								while(1);
}

void create_idle_process()
{
								idle_task = alloc_new_task(FALSE);
								idle_task->task_state = IDLE_STATE;
								kstrcpy(idle_task->comm, "IDLE Process");
								kstrcpy(idle_task->cwd, "/");
								schedule_process(idle_task, (uint64_t)idle_process, (uint64_t)&idle_task->kernel_stack[KERNEL_STACK_SIZE-1]);
}

static void add_to_task_list(task_struct* new_task)
{
								task_struct* ready_list_ptr = next_task_list;
if (new_task->task_state == IDLE_STATE) {
        return;
    } else if (new_task->task_state == EXIT_STATE) {
        add_to_task_free_list(new_task);
        return;
    } else if (new_task->task_state == RUNNING_STATE) {
        new_task->task_state = READY_STATE;
    }


								if (ready_list_ptr == NULL) {
																next_task_list = new_task;
								} else {
																while (ready_list_ptr->next != NULL) {
																								ready_list_ptr = ready_list_ptr->next;
																}
																ready_list_ptr->next = new_task;
																new_task->next = NULL;
								}
}

static task_struct* get_next_ready_task()
{
								task_struct* last, *next;

								CURRENT_TASK = next_task_list;
								last = next = NULL;

								// Find a process with READY_STATE
								while (CURRENT_TASK != NULL) {
																if (CURRENT_TASK->task_state == READY_STATE) {
																								next = CURRENT_TASK;
																								next->task_state = RUNNING_STATE;
																								break;
																}
																last = CURRENT_TASK;
																CURRENT_TASK = CURRENT_TASK->next;
								}

								if (CURRENT_TASK == NULL) {
																// Schedule IDLE process if no READY process found
																CURRENT_TASK = next = idle_task;
								} else {
																// READY process found
																if (last != NULL) {
																								last->next = CURRENT_TASK->next;
																								CURRENT_TASK->next = next_task_list;
																} else {
																								next_task_list = next_task_list->next;
																}
								}
								return next;
}

// Keeps track of the sleep time for the sleeping processes
static void sleep_time_check()
{
								task_struct* timer_list_ptr = next_task_list;

								while (timer_list_ptr != NULL) {
																if (timer_list_ptr->task_state == SLEEP_STATE) {
                        //kprintf("%d", timer_list_ptr->sleep_time);
																								if (timer_list_ptr->sleep_time == 0) {
																																timer_list_ptr->task_state = READY_STATE;
																								} else {
																																timer_list_ptr->sleep_time -= 1;  
																								}
																}
																timer_list_ptr = timer_list_ptr->next;
								}
}

#define switch_to_ring3 \
								__asm__ __volatile__(\
																								"movq $0x23, %rax;"\
																								"movq %rax,  %ds;"\
																								"movq %rax,  %es;"\
																								"movq %rax,  %fs;"\
																								"movq %rax,  %gs;")
extern void irq0();

void schedule_process(task_struct* new_task, uint64_t entry_point, uint64_t stack_top)
{
								// 1) Set up kernel stack => ss, rsp, rflags, cs, rip
								if (new_task->IsUserProcess) {
																new_task->kernel_stack[KERNEL_STACK_SIZE-1] = 0x23;
																new_task->kernel_stack[KERNEL_STACK_SIZE-4] = 0x1b;
								} else {
																new_task->kernel_stack[KERNEL_STACK_SIZE-1] = 0x10;
																new_task->kernel_stack[KERNEL_STACK_SIZE-4] = 0x08;
								}
								new_task->kernel_stack[KERNEL_STACK_SIZE-2] = stack_top;
								new_task->kernel_stack[KERNEL_STACK_SIZE-3] = 0x200202UL;
								new_task->kernel_stack[KERNEL_STACK_SIZE-5] = entry_point;
								new_task->rip_register = entry_point;

								// 2) Leave 9 spaces for POPA => KERNEL_STACK_SIZE-6 to KERNEL_STACK_SIZE-20

								// 3) Set return address to POPA in irq0()
								new_task->kernel_stack[KERNEL_STACK_SIZE-21] = (uint64_t)irq0 + 0x20;

								// 4) Set rsp to KERNEL_STACK_SIZE-16
								new_task->rsp_register = (uint64_t)&new_task->kernel_stack[KERNEL_STACK_SIZE-22];

								// 5) Add to the next_task_list 
								add_to_task_list(new_task);
}

task_struct* copy_task_struct(task_struct* parent_task)
{
								task_struct* child_task  = alloc_new_task(TRUE);
								uint64_t parent_pml4_t   = parent_task->mm->pml4_t;
								uint64_t child_pml4_t    = child_task->mm->pml4_t;
								vma_struct *parent_vma_l = parent_task->mm->vma_list;
								vma_struct *child_vma_l  = NULL;

								// Copy contains of parent mm_struct, except pml4_t and vma_list
								memcpy((void*)child_task->mm, (void*)parent_task->mm, sizeof(mm_struct));

								child_task->mm->pml4_t   = child_pml4_t;
								child_task->mm->vma_list = NULL; 

								for (int i = 0; i < MAXFD; ++i) {
																if (parent_task->file_descp[i] != NULL) {
																								FD* file_d                = (FD *)kmalloc(sizeof(FD));
																								file_d->filenode          = ((FD *)(parent_task->file_descp[i]))->filenode;
																								file_d->curr              = ((FD *)(parent_task->file_descp[i]))->curr;
																								child_task->file_descp[i] = (uint64_t *)file_d;
																}
								}

								child_task->ppid   = parent_task->pid;
								child_task->parent = parent_task;
							kstrcpy((void*)child_task->comm, (void*)parent_task->comm);
							kstrcpy((void*)child_task->cwd, (void*)parent_task->cwd);

								add_child_to_parent(child_task);

								while (parent_vma_l) {
																uint64_t start, end;
																uint64_t vaddr, paddr; 
																uint64_t *pte_entry, page_flags;

																start = parent_vma_l->vm_start;
																end   = parent_vma_l->vm_end;  

																if (child_task->mm->vma_list == NULL) {
																								child_task->mm->vma_list = alloc_new_vma(start, end, parent_vma_l->vm_flags, parent_vma_l->vm_type, parent_vma_l->vm_file_descp);
																								child_vma_l              = child_task->mm->vma_list;
																} else {
																								child_vma_l->vm_next = alloc_new_vma(start, end, parent_vma_l->vm_flags, parent_vma_l->vm_type, parent_vma_l->vm_file_descp);
																								child_vma_l          = child_vma_l->vm_next;
																}

																// Deep Copy allocation for stack VMA only
																if (parent_vma_l->vm_type == STACK) {

																								uint64_t k_vaddr = get_top_virtaddr();
																								uint64_t *k_pte_entry;

																								vaddr = PAGE_ALIGN(end) - 0x1000;
																								while (vaddr >= start) {
																																LOAD_CR3(parent_pml4_t);

																																pte_entry = get_pte_entry(vaddr);
																																if (!IS_PRESENT_PAGE(*pte_entry))
																																								break;

																																// Allocate a new page in kernel
																																paddr = phys_alloc_block();
																																map_virt_phys_addr(k_vaddr, paddr, RW_USER_FLAGS);

																																//kprintf("\nStack v:%p p:%p", vaddr, paddr);
																																// Copy parent page in kernel space
																																memcpy((void*)k_vaddr, (void*)vaddr, PAGESIZE);

																																// Map paddr with child vaddr
																																LOAD_CR3(child_pml4_t);
																																//kprintf("\nStack v:%p p:%p", vaddr, paddr);
																																map_virt_phys_addr(vaddr, paddr, RW_USER_FLAGS);

																																// Unmap k_vaddr
																																k_pte_entry  = get_pte_entry(k_vaddr);
																																*k_pte_entry = 0UL;

																																vaddr = vaddr - PAGESIZE;
																								}

																} else {
																								// Share same physical pages by setting COW and READONLY bits

																								vaddr = PAGE_ALIGN(start);
																								while (vaddr < end) {
																																LOAD_CR3(parent_pml4_t);

																																pte_entry = get_pte_entry(vaddr);

																																if (IS_PRESENT_PAGE(*pte_entry)) {
																																								unset_writable_bit(pte_entry);
																																								set_cow_bit(pte_entry);

																																								paddr      = *pte_entry & PAGING_ADDR;
																																								page_flags = *pte_entry & PAGING_FLAGS;

																																								LOAD_CR3(child_pml4_t);
																																								//kprintf("\nv:%p p:%p f:%p", vaddr, paddr, page_flags);
																																								map_virt_phys_addr(vaddr, paddr, page_flags);
																																								phys_inc_block_ref(paddr);
																																}
																																vaddr = vaddr + PAGESIZE;
																								}
																}
																LOAD_CR3(parent_pml4_t);
																parent_vma_l = parent_vma_l->vm_next;
								}

								return child_task;
}

void schedule()
{
        if (next_task_list == NULL) {
            kprintf("\nReady List Empty");
            return;
        }
    if (IsInitSchedule == FALSE) {
        prev = next_task_list;
        next_task_list = next_task_list->next;
        CURRENT_TASK = prev;
    } else {
        prev = CURRENT_TASK;
        CURRENT_TASK = next_task_list;
        next = next_task_list;
        // Add prev to the end of the READY_LIST
        add_to_task_list(prev);
        //kprintf("\nThe task to be scheduled next is %p", next_task_list);
        next_task_list = next_task_list->next;
        //kprintf("\nThe task to be scheduled after that is %p", next_task_list);
        switch_to(prev, next);
  }
}

void initSchedule(){
    IsInitSchedule = FALSE;
    schedule();
      __asm__ __volatile("movq %[prev_sp], %%rsp"::[prev_sp] "m" (prev->rsp_register));
     __asm__ __volatile("movq %[prev_bp], %%rbp"::[prev_bp] "m" (prev->rsp_register));
    IsInitSchedule = TRUE;
}

void createKernelProcess(uint64_t funcAddr){
#if  0
								task_struct* task = alloc_new_task(FALSE);
								task->rip_register = funcAddr;
								task->task_state = RUNNING_STATE;
								kstrcpy(task->comm, "test");
								schedule_process(task, (uint64_t)funcAddr, (uint64_t)&task->kernel_stack[KERNEL_STACK_SIZE-1]);
#endif
								task_struct* new_task = alloc_new_task(FALSE);
								new_task->task_state = RUNNING_STATE;
								kstrcpy(new_task->comm, "test");
								kstrcpy(new_task->cwd, "/");
int stackind = KERNEL_STACK_SIZE-1;
new_task->rsp_register = (uint64_t)&new_task->kernel_stack[stackind];
new_task->rip_register = funcAddr;
    new_task->next = NULL;
    new_task->last = NULL;

    // Add to the ready list 
    add_to_task_list(new_task);
}
void timerHandler(){
    tick++;
    if(tick%100 == 0) displayTime();

if (IsInitSchedule) {
        sleep_time_check();

        if (CURRENT_TASK == NULL) {
            next = get_next_ready_task();

            LOAD_CR3(next->mm->pml4_t);

            // Switch the kernel stack to that of the first process
            __asm__ __volatile__("movq %[next_rsp], %%rsp" : : [next_rsp] "m" (next->rsp_register));

            if (next->IsUserProcess) {
                set_tss_rsp((uint64_t)&next->kernel_stack[KERNEL_STACK_SIZE-1]);
                switch_to_ring3;
            }

#if DEBUG_SCHEDULING
            kprintf("\nScheduler Initiated with PID: %d[%d]", next->pid, next->task_state);
#endif

        } else {
            uint64_t cur_rsp;
            __asm__ __volatile__("movq %%rsp, %[cur_rsp]": [cur_rsp] "=r"(cur_rsp));

            prev = CURRENT_TASK;
            prev->rsp_register = cur_rsp;

            // Add prev to the end of the next_task_list for states other than EXIT
            add_to_task_list(prev);

            // Schedule next READY process
            next = get_next_ready_task();

            // Context Switch only if next process is different than current process
            if (prev != next) {

                LOAD_CR3(next->mm->pml4_t);
                __asm__ __volatile__("movq %[next_rsp], %%rsp" : : [next_rsp] "m" (next->rsp_register));

                if (next->IsUserProcess) {
                    set_tss_rsp((uint64_t)&next->kernel_stack[KERNEL_STACK_SIZE-1]);
                    switch_to_ring3;
                }
                //kprintf(" %d[%d]", next->pid, next->task_state);
            }
        }
    }
    outb(0x20,0x20);
}
