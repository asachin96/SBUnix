#include <master.h>

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

extern void isr14();
extern void irq0();
extern void irq1();
extern void syscall_handler();

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

void installIdt(){
	initPic();
	idtPtr.size = sizeof(struct IdtTable)*256 -1;
	idtPtr.addr = (uint64_t)&interruptArray; 
	setInteruptHandler(14, (uint64_t)&isr14);
	setInteruptHandler(32, (uint64_t)&irq0);
	setInteruptHandler(33, (uint64_t)&irq1);
	setInteruptHandler(128, (uint64_t)&syscall_handler);
	lidt(&idtPtr);
}

