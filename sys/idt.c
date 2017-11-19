#include <sys/idt.h>
#include <sys/util.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

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
extern void isr32();
extern void isr33();
extern void isrcommon();

void commonHandler(){
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
		interruptArray[interruptNo].dpl = 0;
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
		setInteruptHandler(0, (uint64_t)&isr0);
		setInteruptHandler(1, (uint64_t)&isrcommon);
		setInteruptHandler(2, (uint64_t)&isrcommon);
		setInteruptHandler(3, (uint64_t)&isrcommon);
		setInteruptHandler(4, (uint64_t)&isrcommon);
		setInteruptHandler(5, (uint64_t)&isrcommon);
		setInteruptHandler(6, (uint64_t)&isrcommon);
		setInteruptHandler(7, (uint64_t)&isrcommon);
		setInteruptHandler(8, (uint64_t)&isrcommon);
		setInteruptHandler(9, (uint64_t)&isrcommon);
		setInteruptHandler(10, (uint64_t)&isrcommon);
		setInteruptHandler(11, (uint64_t)&isrcommon);
		setInteruptHandler(12, (uint64_t)&isrcommon);
		setInteruptHandler(13, (uint64_t)&isrcommon);
		setInteruptHandler(14, (uint64_t)&isrcommon);
		setInteruptHandler(15, (uint64_t)&isrcommon);
		setInteruptHandler(16, (uint64_t)&isrcommon);
		setInteruptHandler(17, (uint64_t)&isrcommon);
		setInteruptHandler(18, (uint64_t)&isrcommon);
		setInteruptHandler(19, (uint64_t)&isrcommon);
		setInteruptHandler(20, (uint64_t)&isrcommon);
		setInteruptHandler(21, (uint64_t)&isrcommon);
		setInteruptHandler(22, (uint64_t)&isrcommon);
		setInteruptHandler(23, (uint64_t)&isrcommon);
		setInteruptHandler(24, (uint64_t)&isrcommon);
		setInteruptHandler(25, (uint64_t)&isrcommon);
		setInteruptHandler(26, (uint64_t)&isrcommon);
		setInteruptHandler(27, (uint64_t)&isrcommon);
		setInteruptHandler(28, (uint64_t)&isrcommon);
		setInteruptHandler(29, (uint64_t)&isrcommon);
		setInteruptHandler(30, (uint64_t)&isrcommon);
		setInteruptHandler(31, (uint64_t)&isrcommon);
		setInteruptHandler(33, (uint64_t)&isr33);
		setInteruptHandler(32, (uint64_t)&isr32);
		lidt(&idtPtr);
}

