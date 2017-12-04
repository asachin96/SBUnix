#include <sys/idt.h>
#include <sys/util.h>
#include <sys/defs.h>
#include <sys/types.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <syscall.h>
#include <sys/virt_mm.h>
#include <sys/proc_mngr.h>
#include <sys/idt.h>

struct IdtTable{
		uint16_t offset_low;
		uint16_t selector;
		unsigned ist : 3 ;
		unsigned reserved0 : 5;
		unsigned type : 4;
		unsigned zero : 1;
		unsigned dpl : 2;
		unsigned p : 1;
		uint16_t offset_middle;
		uint32_t offset_high;
		uint32_t reserved1;
}__attribute__((packed));

struct IdtPtr {
		uint16_t size;
		uint64_t addr;
}__attribute__((packed));

static struct IdtTable interruptArray[256];
static struct IdtPtr idtPtr; 

void lidt(struct IdtPtr* idtr);

extern void isr0();
extern void isr10();
extern void isr13();
extern void isr14();
extern void irq0();
extern void irq1();
extern void isr32();
//extern void isr33();
extern void isrcommon();
extern void syscall_handler();
extern void sys_exit();
extern uint64_t* get_pte_entry(uint64_t vaddr);
extern int phys_get_block_ref(uint64_t paddr);
extern uint64_t phys_alloc_block();
extern void phys_dec_block_ref(uint64_t paddr);

static void divide_by_zero_handler(registers_t regs)
{
    kprintf("Divide by Zero!");
    sys_exit();
}

/* Invalid TSS exception handler */
static void tss_fault_handler(registers_t regs)
{
    kprintf("Invalid TSS Exception!");
}

/* General Protection fault handler */
static void gpf_handler(registers_t regs)
{
    kprintf("General Protection Fault!");
}

/* Page Fault handler */
static void page_fault_handler(registers_t regs)
{
    uint64_t err_code = regs.err_no;
    uint64_t fault_addr;
    bool IsFault = FALSE;

    READ_CR2(fault_addr);
    //kprintf("\nFault Addr:%p Error Code:%p", fault_addr, err_code);

    if (fault_addr >= KERNEL_START_VADDR) {
        // Page fault in kernel
        //kprintf("\nFault Addr:%p Error Code:%p", fault_addr, err_code);
        kprintf("Page fault in kernel!");
    } else if (err_code & 0x1) {
        // Page is PRESENT
        // Get the PTE entry for the fault address
        uint64_t *pte_entry = get_pte_entry(fault_addr); 
        uint64_t paddr = *pte_entry & PAGING_ADDR;

        //kprintf("\npte_entry = %p\n", *pte_entry);

        if (!IS_WRITABLE_PAGE(*pte_entry) && IS_COW_PAGE(*pte_entry)) {
            // Check if the physical page is referred by more than virtual pages.
            // If YES, allocate a new page, copy the contents and set writable permissions.
            // If NO, use the same physical page, but remove the COW and READONLY bits
            if (phys_get_block_ref(paddr) > 1) {
                uint64_t new_paddr = phys_alloc_block();
                uint64_t tvaddr = get_temp_vaddr(new_paddr);
                memcpy((void*)tvaddr, (void*)PAGE_ALIGN(fault_addr), PAGESIZE);
                *pte_entry = new_paddr | RW_USER_FLAGS;
                free_temp_vaddr(tvaddr);
                phys_dec_block_ref(paddr);
            } else {
                unset_cow_bit(pte_entry);
                set_writable_bit(pte_entry);
            }
        } else {
            IsFault = TRUE;
        }

    } else {
        // Page is not PRESENT and thus was not allocated earlier.
        // If fault addr is within VMA area, mmap the VMA area,
        // Else kill the process and raise an SEGFAULT
        vma_struct* vma_ptr = CURRENT_TASK->mm->vma_list;
        uint64_t start, end;
        while (vma_ptr != NULL) {
            start = vma_ptr->vm_start; end = vma_ptr->vm_end;
            kprintf("\n[VMA]:%p-%p", start, end);
            if (fault_addr >= start && fault_addr < end) {
                kmmap(start, end - start, RW_USER_FLAGS);
                break;
            }
            vma_ptr = vma_ptr->vm_next;
        }
        if (vma_ptr == NULL) {
            IsFault = TRUE;
        }
    }

    if (IsFault) {
        kprintf("\nSegmentation Fault %p (%p) - Process Terminated", fault_addr, err_code);
        sys_exit();
    }
}

void commonHandler(registers_t regs){

    switch (regs.int_no) {
        case 0:
            divide_by_zero_handler(regs);
            break;
        case 10:
            tss_fault_handler(regs);
            break;
        case 13:
            gpf_handler(regs);
            break;
        case 14:
            page_fault_handler(regs);
            break;
        default:
            break;
    }
   kprintf("Interrup received\n");
}

//port 0 of Pic is connected to PIT channel 0 IRQ line
//On interrupt it has to write IRQ no 32 on data bus to be recd by processor
void initPic(){
		outb(0x20, 0x11);
		outb(0xA0, 0x11);
		outb(0x21, 0x20);  
		outb(0xA1, 0x28); 
		outb(0x21, 0x04);
		outb(0xA1, 0x02);
		outb(0x21, 0x01);
		outb(0xA1, 0x01);
		outb(0x21, 0x0);
		outb(0xA1, 0x0);
}

void setInteruptHandler(int interruptNo, uint64_t handler){
		interruptArray[interruptNo].offset_low = handler & 0XFFFF;
		interruptArray[interruptNo].offset_middle= handler >> 16 & 0XFFFF;
		interruptArray[interruptNo].offset_high = handler >> 32 &0XFFFFFFFF;
		interruptArray[interruptNo].selector = 8;
		interruptArray[interruptNo].dpl = (interruptNo == 128)?11:0;
		interruptArray[interruptNo].ist= 0;
		interruptArray[interruptNo].type = 0x0e; //check with 8e
		interruptArray[interruptNo].reserved0 =0;
		interruptArray[interruptNo].reserved1 =0;
		interruptArray[interruptNo].zero = 0;
		interruptArray[interruptNo].p = 1;
}
#if 0
void setInteruptSystemHandler(int interruptNo, uint64_t handler){
		interruptArray[interruptNo].offset_low = handler & 0XFFFF;
		interruptArray[interruptNo].offset_middle= handler >> 16 & 0XFFFF;
		interruptArray[interruptNo].offset_high = handler >> 32 &0XFFFFFFFF;
		interruptArray[interruptNo].selector = 8;
		interruptArray[interruptNo].dpl = 0;
		interruptArray[interruptNo].ist= 0;
		interruptArray[interruptNo].type = 0xee; //check with 8e
		interruptArray[interruptNo].reserved0 =0;
		interruptArray[interruptNo].reserved1 =0;
		interruptArray[interruptNo].zero = 0;
		interruptArray[interruptNo].p = 1;
}
#endif
void installIdt(){
		initPic();
		idtPtr.size = sizeof(struct IdtTable)*256 -1;
		idtPtr.addr = (uint64_t)&interruptArray; 
		setInteruptHandler(0, (uint64_t)&isr0);
		setInteruptHandler(10, (uint64_t)&isr10);
		setInteruptHandler(13, (uint64_t)&isr13);
		setInteruptHandler(14, (uint64_t)&isr14);
		setInteruptHandler(33, (uint64_t)&irq1);
	 setInteruptHandler(128, (uint64_t)&syscall_handler);
		setInteruptHandler(32, (uint64_t)&irq0);
		lidt(&idtPtr);
}

