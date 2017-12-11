//
// isr_handler.c -- High level interrupt service routines and interrupt request handlers.
//

#include <sys/defs.h>
#include <sys/common.h>
#include <sys/irq_common.h>
#include <sys/types.h>
#include <sys/paging.h>
#include <sys/virt_mm.h>
#include <sys/phys_mm.h>
#include <sys/proc_mngr.h>
#include <sys/kprintf.h>

/* Divide by Zero handler */
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
static void handleErrorCode1(uint64_t* fault_addr, bool * isFault)
{
        uint64_t *pte_entry = get_pte_entry(*fault_addr); 
        uint64_t paddr = *pte_entry & PAGING_ADDR;


        if (!IS_WRITABLE_PAGE(*pte_entry) && IS_COW_PAGE(*pte_entry)) {
            if (phys_get_block_ref(paddr) > 1) {
                uint64_t new_paddr = phys_alloc_block();
                uint64_t tvaddr = get_vaddr(new_paddr);
                memcpy((void*)tvaddr, (void*)PAGE_ALIGN(*fault_addr), PAGESIZE);
                *pte_entry = new_paddr | RW_USER_FLAGS;
                free_temp_vaddr(tvaddr);
                phys_dec_block_ref(paddr);
            } else {
                unset_cow_bit(pte_entry);
                set_writable_bit(pte_entry);
            }
        } else {
            *isFault = TRUE;
        }

}
/* Page Fault handler */
static void page_fault_handler(registers_t regs)
{
    uint64_t err_code = regs.err_no;
    uint64_t fault_addr;
    bool IsFault = FALSE;

    READ_CR2(fault_addr);

    if (fault_addr >= KERNEL_START_VADDR) {
        kprintf("Page fault in kernel!");
    } else if (err_code & 0x1) {
							handleErrorCode1(&fault_addr,&IsFault);

    } else {
        vma_struct* vma_ptr = CURRENT_TASK->mm->vma_list;
        uint64_t start, end;
        while (vma_ptr != NULL) {
            start = vma_ptr->vm_start; end = vma_ptr->vm_end;
            //kprintf("\n[VMA]:%p-%p", start, end);
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

// Common handler
void isr_handler(registers_t regs)
{
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
}

