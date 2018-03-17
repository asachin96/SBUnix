#include <sys/defs.h>
#include <sys/util.h>
#include <sys/phys_mm.h>
#include <sys/virt_mm.h>
#include <sys/types.h>
#include <sys/paging.h>

uint64_t topVirtAddr;

uint64_t get_top_virtaddr()
{
    return topVirtAddr;
}

void set_top_virtaddr(uint64_t vaddr)
{
    topVirtAddr = vaddr;
}

void* virt_alloc_pages(uint32_t no_of_vpages, uint64_t flags)
{
    void *ret_addr = NULL; 
    uint64_t physaddr = NULL;
    int i = 0;

    ret_addr = (void*)topVirtAddr; 
    while(i<no_of_vpages){
        physaddr = phys_alloc_block();
        map_virt_phys_addr(topVirtAddr, physaddr, flags); 
        topVirtAddr += PAGESIZE;     
        i++;
    }

    return ret_addr;
}

void free_virt_page(void *vaddr)
{
    uint64_t *pte_entry = NULL;
    uint64_t physaddr = NULL;

    pte_entry = get_pte_entry((uint64_t)vaddr);
    physaddr = *pte_entry & PAGING_ADDR;     
    phys_free_block(physaddr, TRUE);
    *pte_entry = 0;
}

uint64_t get_vaddr(uint64_t paddr)
{
    uint64_t vaddr = get_top_virtaddr();

    map_virt_phys_addr(vaddr, paddr, RW_USER_FLAGS);

    return vaddr;
}

void free_temp_vaddr(uint64_t vaddr)
{
    uint64_t *k_pte_entry = get_pte_entry(vaddr);
    *k_pte_entry = 0UL;
}

void zero_out_phys_block(uint64_t paddr)
{
    uint64_t vaddr = get_vaddr(paddr);
    memset((void*)vaddr, 0, PAGESIZE);
    free_temp_vaddr(vaddr);
}
