//
// irq_handler.c -- High level interrupt service routines and interrupt request handlers
//                  for IRQ0 to IRQ15 
//

#include <sys/defs.h>
#include <syscall.h>
#include <sys/irq_common.h>
#include <sys/types.h>
#include <sys/util.h>

extern void kbHandler();
void irq_handler(registers_t regs)
{
    switch (regs.int_no) {
        case 32:
            // Called directly from irq0()
            break;
        case 33:
            kbHandler();
            break;
        default:
            break;
    }
    // Send EOI signal to Master PIC
    outb(0x20, 0x20);
}

