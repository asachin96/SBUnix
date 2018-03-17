#ifndef _MASTER_H
#define _MASTER_H
#include <sys/defs.h>
#include <sys/procMgr.h>
#include <sys/timer.h>
#include <dirent.h>
#include <sys/elf.h>
#include <sys/util.h>
#include <syscall.h>
#include <sys/memoryMgr.h>
#include <sys/kprintf.h>
#include <sys/kBoard.h>
#include <sys/idt.h>

#define STACKOFFSET 0x1000
#define KERNEL_START_VADDR 0xFFFFFFFF80000000UL
#define KERNEL_STACK_SIZE 512
#define getAddr(addr)        addr & 0x000FFFFFFFFFF000UL
#define getFlags(addr)        addr & 0xFFF0000000000FFFUL
#define PML4 1
#define PDPE 2
#define PDE 3
#define PTE 4
#define PAGESIZE 0x1000
#define true 1
#define false 0

extern uint64_t *getEntry(uint64_t, int);
extern void displayTime();
extern void set_tss_rsp();
extern int tick;
extern uint64_t nextAddr;
extern task* taskList, *currentTask, *idleTask;
extern bool IsInitSchedule;
extern int nextAvailablePid;
extern void irq0();
extern task* idleTask;
extern uint64_t kernelPml;
void sys_exit();
void init_gdt();
void init_idt();
void init_pic();
void init_tss();

#endif
