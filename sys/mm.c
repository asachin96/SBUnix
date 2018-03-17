#include <sys/common.h>
#include <sys/proc_mngr.h>
#include <sys/paging.h>
#include <sys/virt_mm.h>
#include <sys/tarfs.h>
#include <sys/elf.h>
#include <sys/kmalloc.h>
#include <sys/types.h>
#include <sys/kstring.h>

static uint64_t next_pid = 0;

void set_next_pid(pid_t fnext_pid)
{
								next_pid = fnext_pid;
}

static task_struct *task_free_list = NULL;
static vma_struct *vma_free_list = NULL;

static task_struct* get_free_task_struct()
{
								task_struct *ret = NULL;
								if (task_free_list != NULL) {
																ret = task_free_list;
																task_free_list = task_free_list->next;
								}
								return ret;
}

static vma_struct* get_free_vma_struct()
{
								vma_struct *ret = NULL;
								if (vma_free_list != NULL) {
																ret = vma_free_list;
																vma_free_list = vma_free_list->vm_next;
								}
								return ret;
}

void add_to_task_free_list(task_struct* free_task)
{
								free_task->next = task_free_list;
								task_free_list = free_task;
}

void add_to_vma_free_list(vma_struct* free_vma)
{
								free_vma->vm_next = vma_free_list;
								vma_free_list = free_vma;
}

task_struct* alloc_new_task(bool IsUserProcess)
{
								task_struct* new_task = NULL;

								if ((new_task = get_free_task_struct()) == NULL) {

																mm_struct* mms = (mm_struct *) kmalloc(sizeof(mm_struct));
																mms->pml4_t = create_new_pml4();
																mms->vma_list   = NULL;
																mms->vma_count  = NULL;
																mms->hiwater_vm = NULL;
																mms->total_vm   = NULL;
																mms->stack_vm   = NULL;

																new_task    = (task_struct*) kmalloc(sizeof(task_struct));
																new_task->mm = mms;
																new_task->next          = NULL;
																new_task->last          = NULL;
																new_task->parent        = NULL;
																new_task->childhead     = NULL;
																new_task->siblings      = NULL;
																new_task->no_children   = 0;
																new_task->wait_on_child_pid = 0;
																memset((void*)new_task->kernel_stack, 0, KERNEL_STACK_SIZE);
																memset((void*)new_task->file_descp, 0, MAXFD*8);
								}

								new_task->pid           = next_pid++;
								new_task->ppid          = 0;
								new_task->IsUserProcess = IsUserProcess;
								new_task->task_state    = READY_STATE;

								return new_task;
}

vma_struct* alloc_new_vma(uint64_t start_addr, uint64_t end_addr, uint64_t flags, uint64_t type, uint64_t file_d)
{
								vma_struct *vma = NULL;

								if ((vma = get_free_vma_struct()) == NULL) {
																vma = (vma_struct*) kmalloc(sizeof(vma_struct));
								}
								vma->vm_start       = start_addr;
								vma->vm_end         = end_addr; 
								vma->vm_flags       = flags;
								vma->vm_type        = type; 
								vma->vm_next        = NULL;
								vma->vm_file_descp  = file_d;
								return vma;
}

void empty_task_struct(task_struct *cur_task)
{
								mm_struct *mms = cur_task->mm;

								empty_vma_list(mms->vma_list);
								empty_page_tables(mms->pml4_t);
								mms->vma_list   = NULL;
								mms->vma_count  = NULL;
								mms->hiwater_vm = NULL;
								mms->total_vm   = NULL;
								mms->stack_vm   = NULL;

								memset((void*)cur_task->kernel_stack, 0, KERNEL_STACK_SIZE);
								memset((void*)cur_task->file_descp, 0, MAXFD*8);
								cur_task->next        = NULL;
								cur_task->last        = NULL;
								cur_task->parent      = NULL;
								cur_task->childhead   = NULL;
								cur_task->siblings    = NULL;
								cur_task->no_children = 0;
								cur_task->wait_on_child_pid = 0;
}

void empty_vma_list(vma_struct *vma_list)
{
								vma_struct *cur_vma  = vma_list;
								vma_struct *last_vma = NULL;

								while (cur_vma) {

																cur_vma->vm_mm    = NULL;
																cur_vma->vm_start = NULL;
																cur_vma->vm_end   = NULL;
																cur_vma->vm_flags = NULL;
																cur_vma->vm_file_descp = NULL;

																last_vma = cur_vma;
																cur_vma = cur_vma->vm_next;
								}

								if (last_vma) {
																last_vma->vm_next = vma_free_list;
																vma_free_list = vma_list;
								}
}

void replace_child_task(task_struct *old_task, task_struct *new_task)
{
								task_struct *parent_task = old_task->parent;
								task_struct *sibling_l, *last_sibling;

								sibling_l = parent_task->childhead;
								last_sibling = NULL;
								while (sibling_l) {
																if (sibling_l == old_task) {
																								break;
																}
																last_sibling = sibling_l;
																sibling_l = sibling_l->siblings;
								}

								if (!sibling_l) return; 

								if (last_sibling) {
																last_sibling->siblings = new_task;
																new_task->siblings = sibling_l->siblings;
								} else {
																new_task->siblings = sibling_l->siblings;
																parent_task->childhead = new_task;
								}
}

bool is_addr_valid(task_struct *proc, uint64_t addr, uint64_t size)
{
								mm_struct *mms = proc->mm;
								vma_struct *iter;

								for (iter = mms->vma_list; iter != NULL; iter = iter->vm_next) {

																if (addr < iter->vm_start && (addr + size) > iter->vm_start)
																								return 0;

																if (addr < iter->vm_end && (addr + size) > iter->vm_end)
																								return 0;
								}

								if (addr >= KERNEL_START_VADDR)
																return 0;

								return 1;
}

void increment_brk(task_struct *proc, uint64_t bytes)
{   
								mm_struct *mms = proc->mm;
								vma_struct *iter;

								for (iter = mms->vma_list; iter != NULL; iter = iter->vm_next) {
																if (iter->vm_type == HEAP) {
																								iter->vm_end  += bytes;
																								mms->end_brk  += bytes; 
																								mms->total_vm += bytes;
																								break;
																}
								}
}

void* kmmap(uint64_t start_addr, int bytes, uint64_t flags)
{
								int no_of_pages = 0;
								uint64_t end_vaddr;
								uint64_t cur_top_vaddr = get_top_virtaddr();

								set_top_virtaddr(PAGE_ALIGN(start_addr));
								end_vaddr = start_addr + bytes;
								no_of_pages = ((end_vaddr-1) >> 12) - ((start_addr) >> 12) + 1;
								virt_alloc_pages(no_of_pages, flags);
								set_top_virtaddr(cur_top_vaddr);

								return (void*)start_addr;
}

int vmafind(vma_struct *iter, vma_struct* temp, uint64_t addr, uint64_t nbytes){
								int flag = 0;
								while (iter->vm_next != NULL) {
																temp = iter;            
																iter = iter->vm_next;

																if (temp->vm_end < addr && (iter->vm_start > addr + nbytes)) {
																								flag = 1;
																								break;
																}
								}
								return flag;
}

vma_struct* vmalogic(uint64_t addr, uint64_t nbytes, uint64_t flags, uint64_t type, uint64_t file_d)
{
								vma_struct *node, *iter, *temp=NULL;

								node = alloc_new_vma(addr, addr + nbytes, flags, type, file_d); 
								CURRENT_TASK->mm->vma_count++;
								CURRENT_TASK->mm->total_vm += nbytes;
								iter = CURRENT_TASK->mm->vma_list;

								if (vmafind(iter, temp,addr, nbytes) == 1) {
																temp->vm_next = node;
																node->vm_next = iter;
								} else {
																iter->vm_next = node; 
								}

								return node;
}
