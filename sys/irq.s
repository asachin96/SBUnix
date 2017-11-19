.macro PUSHA1
    pushq %rax      
    pushq %rbx      
    pushq %rcx      
    pushq %rdx      
    pushq %rsp      
    pushq %rbp      
    pushq %rsi      
    pushq %rdi      
.endm

.macro POPA1
    popq %rdi         
    popq %rsi         
    popq %rbp         
    popq %rsp         
    popq %rdx         
    popq %rcx         
    popq %rbx         
    popq %rax         
.endm

.macro pusha
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

.macro popa
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

.extern timerHandler
.extern kbHandler
.extern commonHandler

.global isr0
.global isr32
.global isr33
.global isrcommon

.text

irq_common:
    PUSHA1
    callq commonHandler
    POPA1
    add $0x10, %rsp
    sti
    iretq
 
isr0:
    cli
    pushq $0x0
    pushq $0x20
    jmp irq_common

isrcommon:
    cli
    addq $8, %rsp	
    pusha
    callq commonHandler
    popa
    sti
    iretq

isr32:
    cli
    pusha
    callq timerHandler
    popa
    sti
    iretq

isr33:
    cli
    pusha
    callq kbHandler
    popa
    sti
    iretq
