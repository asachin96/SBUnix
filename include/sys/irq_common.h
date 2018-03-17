//
// irq_common.h -- Interface and structures for high level interrupt service routines
//

#include <sys/defs.h>

struct registers
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rsi, rbp, rdx, rcx, rbx, rax, rdi; // Registers pushed by PUSHA
    uint64_t int_no, err_no;                            // Interrupt number and error code
    uint64_t rip, cs, rflags, rsp, ss;                  // Registers pushed by the IRETQ 
} __attribute__((packed));

typedef struct registers registers_t;

void init_timer(uint32_t);
void init_keyboard();

