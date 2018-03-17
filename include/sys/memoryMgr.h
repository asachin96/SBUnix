#define PAGE_P 0x01
#define PAGE_UR 0x05
#define PAGE_URW 0x07
#define PAGE_COW 0x100
#include <sys/defs.h>

void initFrame(uint64_t, uint64_t);
uint64_t getFrame(); 
void freeFrame(uint64_t*);
void setupPageTable(uint64_t, uint64_t, uint64_t);
void virtToPhyMapper(uint64_t,uint64_t, uint64_t);
void allocatePages(uint64_t*, uint64_t, int);
void unmapPteEntry(uint64_t);
uint64_t newPml();
