.macro PUSHA
    pushq %rdi
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
.endm

.macro POPA
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rsi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    popq %rdi
.endm

.text

.extern kbHandler
.extern timerHandler
.extern handlePageFault

.global irq0
.global irq1
.global isr14

isr14:
    cli
    pushq $14
    jmp isrCommon

isrCommon:
    PUSHA
    movq %rsp, %rdi
    callq handlePageFault
    POPA
    add $0x10, %rsp
    sti
    iretq

irqCommon:
    PUSHA
    movq %rsp, %rdi
    callq kbHandler
    POPA
    add $0x10, %rsp
    sti
    iretq

irq0:
    cli
    PUSHA
    movq %rsp, %rdi
    callq timerHandler
    POPA
    sti
    iretq
 
irq1:
    cli
    pushq $0x0
    pushq $0x21
    jmp irqCommon

