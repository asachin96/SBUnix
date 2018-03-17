#ifndef _PROC_MNGR_H
#define _PROC_MNGR_H

#include <sys/types.h>

#define USER_STACK_TOP  0xF000000000UL
#define USER_STACK_SIZE 0x10000         // 16*4KB
#define KERNEL_STACK_SIZE 512           // (512*8) = 4KB
#define DEBUG_SCHEDULING 0
#define MAXFD 10

enum task_states {
    RUNNING_STATE,
    READY_STATE,
    SLEEP_STATE,
    WAIT_STATE,
    IDLE_STATE,
    EXIT_STATE,
    ZOMBIE_STATE,
    NUM_TASK_STATES
};

enum vmatype {
    TEXT,
    DATA,
    HEAP,
    STACK,
    ANON,
    FILETYPE,
    NOTYPE
};

enum vmaflag {
    NONE,  //no permission
    X,     //execute only
    W,     //write only
    WX,    //write execute
    R,     //read only
    RX,    //read execute
    RW,    //read write
    RWX    //read write execute
};


typedef struct vm_area_struct vma_struct;
typedef struct mm_struct mm_struct;
typedef struct task_struct task_struct;

struct vm_area_struct {
    mm_struct *vm_mm;               // The address space we belong to.
    uint64_t vm_start;              // Our start address within vm_mm
    uint64_t vm_end;                // The first byte after our end address within vm_mm
    vma_struct *vm_next;            // linked list of VM areas per task, sorted by address
    uint64_t vm_flags;              // Flags read, write, execute permissions
    uint64_t vm_type;               // type of segment its reffering to 
    uint64_t vm_file_descp;         // reference to file descriptors for file opened for writing
};

struct mm_struct {
    vma_struct *vma_list;           // list of VMAs
    uint64_t pml4_t;                // Actual physical base addr for PML4 table
    uint32_t vma_count;             // number of VMAs
    uint64_t hiwater_vm;            // High-water virtual memory usage
    uint64_t total_vm;
    uint64_t stack_vm;
    uint64_t start_brk, end_brk, start_stack;
    uint64_t arg_start, arg_end, env_start, env_end;
};

struct task_struct {
    pid_t pid;
    pid_t ppid;
    bool IsUserProcess;
    uint64_t kernel_stack[KERNEL_STACK_SIZE];
    uint64_t rip_register;
    uint64_t rsp_register;
    uint64_t task_state;            // Saves the current state of task
    mm_struct* mm; 
    char comm[30];                  // Name of task
    char cwd[250];                   // Current working directory
    uint32_t sleep_time;            // Number of centiseconds to sleep
    task_struct* next;              // The next process in the process list
    task_struct* last;              // The process that ran last
    task_struct* parent;            // Keep track of parent process on fork
    task_struct* childhead;         // Keep track of its children on fork
    task_struct* siblings;          // Keep track of its siblings (children of same parent)
    uint64_t* file_descp[MAXFD];    // array of file descriptor pointers
    uint32_t no_children;           // Number of children
    pid_t wait_on_child_pid;        // pid of child last exited
};

extern task_struct* CURRENT_TASK;
void yield(void);
void createKernelProcess(uint64_t funcAddr);
void create_idle_process();
void* kmmap(uint64_t start_addr, int bytes, uint64_t flags);
void schedule_process(task_struct* new_task, uint64_t entry_point, uint64_t stack_top);
void set_tss_rsp0(uint64_t rsp);
void set_next_pid(pid_t fnext_pid);
void replace_child_task(task_struct *old_task, task_struct *new_task);
bool is_addr_valid(task_struct *proc, uint64_t addr, uint64_t size);
void increment_brk(task_struct *proc, uint64_t bytes);

task_struct* alloc_new_task(bool IsUserProcess);
task_struct* copy_task_struct(task_struct* parent_task);
void add_to_task_free_list(task_struct* free_task);
void empty_task_struct(task_struct *cur_task);

vma_struct* alloc_new_vma(uint64_t start_addr, uint64_t end_addr, uint64_t flags, uint64_t type, uint64_t fd_type);
void add_to_vma_free_list(vma_struct* free_vma);
void empty_vma_list(vma_struct *vma_list);

// Syscalls
pid_t sys_getpid();
pid_t sys_getppid();
void sys_exit();
vma_struct* vmalogic(uint64_t addr, uint64_t nbytes, uint64_t flags, uint64_t type, uint64_t file_d);

#endif
