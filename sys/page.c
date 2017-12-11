#include <sys/defs.h>
//#include <sys/common.h>
#include <sys/types.h>
#include <sys/paging.h>
#include <sys/phys_mm.h>
#include <sys/virt_mm.h>
#include <sys/kmalloc.h>
#include <sys/proc_mngr.h>

#define ENTRIES_PER_PTE  512
#define ENTRIES_PER_PDE  512
#define ENTRIES_PER_PDPE 512
#define ENTRIES_PER_PML4 512

#define PTE_SELF_REF  0xFFFFFF0000000000UL
#define PDE_SELF_REF  0xFFFFFF7F80000000UL
#define PDPE_SELF_REF 0xFFFFFF7FBFC00000UL
#define PML4_SELF_REF 0xFFFFFF7FBFDFE000UL

// For accessing Page table addresses, we need to first convert PhyADDR to VirADDR
// So need to add below kernel base address
#define VADDR(PADDR) ((KERNEL_START_VADDR) + PAGE_ALIGN(PADDR))
#define PADDR(VADDR) (PAGE_ALIGN(VADDR) - (KERNEL_START_VADDR))

static uint64_t ker_cr3;
static uint64_t *ker_pml4_t;

uint64_t* get_ker_pml4_t()
{
    return ker_pml4_t;
}

static uint64_t* alloc_pte(uint64_t *pde_table, int pde_off)
{
    uint64_t pte_table = phys_alloc_block();
    pde_table[pde_off] = pte_table | RW_KERNEL_FLAGS;   
    return (uint64_t*)VADDR(pte_table);
} 

static uint64_t* alloc_pde(uint64_t *pdpe_table, int pdpe_off)
{
    uint64_t pde_table = phys_alloc_block();
    pdpe_table[pdpe_off] = pde_table | RW_KERNEL_FLAGS;   
    return (uint64_t*)VADDR(pde_table);
}

static uint64_t* alloc_pdpe(uint64_t *pml4_table, int pml4_off)
{
    uint64_t pdpe_table = phys_alloc_block();
    pml4_table[pml4_off] = pdpe_table | RW_KERNEL_FLAGS;   
    return (uint64_t*)VADDR(pdpe_table);
}

static void init_map_virt_phys_addr(uint64_t vaddr, uint64_t paddr, uint64_t no_of_pages)
{
    uint64_t *pdpe_table = NULL, *pde_table = NULL, *pte_table = NULL;

    int i, j, k, phys_addr, pde_off, pdpe_off, pml4_off , pte_off; 

    pte_off  = (vaddr >> 12) & 0x1FF;
    pde_off  = (vaddr >> 21) & 0x1FF;
    pdpe_off = (vaddr >> 30) & 0x1FF;
    pml4_off = (vaddr >> 39) & 0x1FF;

    phys_addr = (uint64_t) *(ker_pml4_t + pml4_off);
    if (IS_PRESENT_PAGE(phys_addr)) {
        pdpe_table =(uint64_t*) VADDR(phys_addr); 

        phys_addr = (uint64_t) *(pdpe_table + pdpe_off);
        if (IS_PRESENT_PAGE(phys_addr)) {
            pde_table =(uint64_t*) VADDR(phys_addr); 

            phys_addr  = (uint64_t) *(pde_table + pde_off);
            if (IS_PRESENT_PAGE(phys_addr)) {
                pte_table =(uint64_t*) VADDR(phys_addr); 
            } else {
                pte_table = alloc_pte(pde_table, pde_off);
            }
        } else {
            pde_table = alloc_pde(pdpe_table, pdpe_off);
            pte_table = alloc_pte(pde_table, pde_off);
        }
    } else {
        pdpe_table = alloc_pdpe(ker_pml4_t, pml4_off);
        pde_table = alloc_pde(pdpe_table, pdpe_off);
        pte_table = alloc_pte(pde_table, pde_off);
    }

    phys_addr = paddr;  

    if (no_of_pages + pte_off <= ENTRIES_PER_PTE) {
        for (i = pte_off; i < (pte_off + no_of_pages); i++) {
            pte_table[i] = phys_addr | RW_KERNEL_FLAGS; 
            phys_addr += PAGESIZE;
        }
    } else {
        int lno_of_pages = no_of_pages, no_of_pte_t;

        for ( i = pte_off ; i < ENTRIES_PER_PTE; i++) {
            pte_table[i] = phys_addr | RW_KERNEL_FLAGS;
            phys_addr += PAGESIZE;
        }
																//TODO 
        lno_of_pages = lno_of_pages - (ENTRIES_PER_PTE - pte_off);
        no_of_pte_t = lno_of_pages/ENTRIES_PER_PTE;

        for (j = 1; j <= no_of_pte_t; j++) {   
            pte_table = alloc_pte(pde_table, pde_off+j);
            for(k = 0; k < ENTRIES_PER_PTE; k++ ) { 
                pte_table[k] = phys_addr | RW_KERNEL_FLAGS;
                phys_addr += PAGESIZE;
            }
        }
        lno_of_pages = lno_of_pages - (ENTRIES_PER_PTE * pte_off);
        pte_table = alloc_pte(pde_table, pde_off+j);
        
        for(k = 0; k < lno_of_pages; k++ ) { 
            pte_table[k] = phys_addr | RW_KERNEL_FLAGS;
            phys_addr += PAGESIZE;
        }
    }
}

void init_paging(uint64_t kernmem, uint64_t physbase, uint64_t no_of_pages)
{
    ker_cr3 = phys_alloc_block();
    ker_pml4_t = (uint64_t*) VADDR(ker_cr3);
    ker_pml4_t[510] = ker_cr3 | RW_KERNEL_FLAGS;   
    init_map_virt_phys_addr(kernmem, physbase, no_of_pages);
    init_map_virt_phys_addr(0xFFFFFFFF800B8000, 0xB8000, 1);
    LOAD_CR3(ker_cr3);
    set_top_virtaddr(kernmem + (no_of_pages * PAGESIZE));
    init_kmalloc();
}

uint64_t* get_pte_entry(uint64_t vaddr)
{
    uint64_t tvaddr;
    uint64_t *addr;
    tvaddr  = vaddr << 16 >> 28 << 3;
    tvaddr = tvaddr | PTE_SELF_REF;
    addr = (uint64_t *)tvaddr; 
    return addr;
} 

static uint64_t* get_pde_entry(uint64_t vaddr)
{
    uint64_t tvaddr;
    uint64_t *addr;
    tvaddr  = vaddr << 16 >> 37 << 3;
    tvaddr = tvaddr | PDE_SELF_REF;
    addr = (uint64_t *)tvaddr; 
    return addr;
} 

static uint64_t* get_pdpe_entry(uint64_t vaddr)
{
    uint64_t tvaddr;
    uint64_t *addr;
    tvaddr  = vaddr << 16 >> 46 << 3;
    tvaddr = tvaddr | PDPE_SELF_REF;
    addr = (uint64_t *)tvaddr; 
    return addr;

}

static uint64_t* get_pml4_entry(uint64_t vaddr)
{
    uint64_t tvaddr;
    uint64_t *addr;
    tvaddr  = vaddr << 16 >> 55 << 3;
    tvaddr = tvaddr | PML4_SELF_REF;
    addr = (uint64_t *)tvaddr; 
    return addr;
}

void map_virt_phys_addr(uint64_t vaddr, uint64_t paddr, uint64_t flags)
{
    uint64_t *pml4_entry, *pdpe_entry, *pde_entry, *pte_entry;
    uint64_t entry; 

    pml4_entry = get_pml4_entry(vaddr);
    pdpe_entry = get_pdpe_entry(vaddr); 
    pde_entry = get_pde_entry(vaddr);   
    pte_entry = get_pte_entry(vaddr);

    entry = (uint64_t) *(pml4_entry);

    if (IS_PRESENT_PAGE(entry)) {
        entry = (uint64_t) *(pdpe_entry);

        if (IS_PRESENT_PAGE(entry)) { 
            entry  = (uint64_t) *(pde_entry);

            if (IS_PRESENT_PAGE(entry)) { 
                entry  = (uint64_t) *(pte_entry);

                if (IS_PRESENT_PAGE(entry)) { 
                    phys_free_block(paddr, FALSE);
                } else {
                    *pte_entry = paddr | flags;
                }

            } else {
                *pde_entry = phys_alloc_block() | RW_USER_FLAGS;
                *pte_entry = paddr | flags;
            }

        } else {
            *pdpe_entry = phys_alloc_block() | RW_USER_FLAGS;
            *pde_entry = phys_alloc_block() | RW_USER_FLAGS;
            *pte_entry = paddr | flags;
        }

    } else {
        *pml4_entry = phys_alloc_block() | RW_USER_FLAGS;
        *pdpe_entry = phys_alloc_block() | RW_USER_FLAGS;
        *pde_entry = phys_alloc_block() | RW_USER_FLAGS;
        *pte_entry = paddr | flags;
    }
}

uint64_t create_new_pml4()
{
    uint64_t virtAddr, physAddr, *new_pml4_t;

    virtAddr = get_top_virtaddr();
    set_top_virtaddr(virtAddr + PAGESIZE);
    physAddr = phys_alloc_block();

    map_virt_phys_addr(virtAddr, physAddr, RW_USER_FLAGS);
    new_pml4_t = (uint64_t *) virtAddr;    
      
    new_pml4_t[511] = ker_pml4_t[511];
    
    new_pml4_t[510] = physAddr | RW_USER_FLAGS;

    return physAddr;
}

void empty_page_tables(uint64_t pml4_t)
{
    uint64_t pml4, pdpe, pde, pte;
    uint64_t *pml4_e, *pdpe_e, *pde_e, *pte_e;

    for (pml4 = 0; pml4 < ENTRIES_PER_PML4-2; pml4++) {
        pml4_e = (uint64_t*) (PML4_SELF_REF | (pml4 << 3));
        if (IS_PRESENT_PAGE(*pml4_e)) {
            for (pdpe = 0; pdpe < ENTRIES_PER_PDPE; pdpe++) {
                pdpe_e = (uint64_t*) (PDPE_SELF_REF | (pml4 << 12) | (pdpe << 3));
                if (IS_PRESENT_PAGE(*pdpe_e)) {

                    for (pde = 0; pde < ENTRIES_PER_PDE; pde++) {
                        pde_e = (uint64_t*) (PDE_SELF_REF | (pml4 << 21) | (pdpe << 12) | (pde << 3));
                        if (IS_PRESENT_PAGE(*pde_e)) {
                            for (pte = 0; pte < ENTRIES_PER_PTE; pte++) {
                                pte_e = (uint64_t*) (PTE_SELF_REF | (pml4 << 30) | (pdpe << 21) | (pde << 12) | (pte << 3));
                                if (IS_PRESENT_PAGE(*pte_e)) {
                                    set_top_virtaddr(get_top_virtaddr() + 0x1000);
                                    phys_free_block(*pte_e & PAGING_ADDR, TRUE);
                                    *pte_e = 0UL;
                                }
                            }
                            phys_free_block(*pde_e & PAGING_ADDR, TRUE);
                            *pde_e = 0UL;
                        }
                    }
                    phys_free_block(*pdpe_e & PAGING_ADDR, TRUE);
                    *pdpe_e = 0UL;
                }
            }
            phys_free_block(*pml4_e & PAGING_ADDR, TRUE);
            *pml4_e = 0UL;
        }
    }
    LOAD_CR3(ker_cr3);
}

