#include <sys/defs.h>
#include <sys/common.h>
#include <syscall.h>
#include <sys/paging.h>
#include <sys/proc_mngr.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/virt_mm.h>
#include <sys/kstring.h>
#include <sys/types.h>
#include <sys/fs.h>
#include <dirent.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>

extern fnode_t* root_node;

DIR* sys_opendir(uint64_t* entry, uint64_t* directory)
{
								char* dir_path = (char *)entry;
								DIR* dir = (DIR *)directory;
								fnode_t *auxnode, *currnode = root_node;
								char *temp = NULL; 
								char *path = (char *)kmalloc(sizeof(char) * kstrlen(dir_path));
								int i;

								kstrcpy(path, dir_path); 
								temp = kstrtok(path, "/");  
								int j=0;
								for (;temp != NULL;j++) {
																auxnode = currnode;

																if (kstrcmp(temp, ".") == 0) {
																								currnode = (fnode_t *)currnode->f_child[0];

																} else if (kstrcmp(temp,"..") == 0) {
																								currnode = (fnode_t *)currnode->f_child[1];

																} else {
																									i=2;
																								while (i < currnode->end){
																																if (kstrcmp(temp, currnode->f_child[i]->f_name) == 0) {
																																								currnode = (fnode_t *)currnode->f_child[i];
																																								break;       
																																}        
																								i++;
																								}
																								if (i == auxnode->end) {
																																dir->curr     = NULL;
																																dir->filenode = NULL;
																																return dir;
																								}
																} 
																temp = kstrtok(NULL, "/");          
								}

								if (currnode->f_type == DIRECTORY) {
																dir->curr     = 2; 
																dir->filenode = currnode; 
								} else {
																dir->curr     = NULL;
																dir->filenode = NULL;
								}
								return dir;
}
void createNode(fnode_t* temp_node,DIR* tempdir,char*path,int*end)
{
																temp_node = (fnode_t *)kmalloc(sizeof(fnode_t));                

																make_node(temp_node, tempdir->filenode, path + *end + 1, 0, 2, DIRECTORY, 0);  
																tempdir->filenode->f_child[tempdir->filenode->end] = temp_node;
																tempdir->filenode->end += 1;
}

int sys_mkdir( uint64_t dir)
{
								char *dirpath = (char *)dir; 

								int dirlength = kstrlen(dirpath);
								char *path;
								DIR* tempdir = kmalloc(sizeof(struct file_dir));
								fnode_t *temp_node;
								tempdir = sys_opendir((uint64_t *)dirpath,(uint64_t *) tempdir);

								if (tempdir->filenode != NULL) 
																return -1;
								

								path = (char *)kmalloc(sizeof(char) * dirlength);
								kstrcpy(path, dirpath);


								int end = dirlength;


								if (path[dirlength - 1] == '/')
																end = dirlength - 2;
								else
																end = dirlength - 1;

								while (dirpath[end] != '/')
																--end; 

								

								path[end] = '\0'; 
								tempdir = sys_opendir((uint64_t *)path, (uint64_t*)tempdir); 
								temp_node = NULL;
								if (tempdir != NULL) {
																createNode(temp_node,tempdir,path,&end);
																return 0; 
								} else 
																return -1; 
								
}

struct dirent* sys_readdir(uint64_t* entry)
{
								DIR *dir = (DIR*)entry;
								if (dir->filenode->end < 3 || dir->curr == dir->filenode->end || dir->curr == 0) { 
																return NULL;
								} else {
																kstrcpy(dir->curr_dirent.name, dir->filenode->f_child[dir->curr]->f_name);
																dir->curr++;
																return &dir->curr_dirent;
								}
}

int sys_closedir(uint64_t* entry)
{
								DIR *dir = (DIR *)entry; 
								if(dir->filenode->f_type == DIRECTORY && dir->curr > 1) {

																dir->filenode = NULL; 
																dir->curr = 0;
																return 0;
								} else {
																return -1; 
								}
}
void handleNonRootCase(char tmp[256],char path[256],char *dir_path)
{
																kstrcpy(tmp, CURRENT_TASK->cwd);
																kstrcat(tmp,"/");
																kstrcat(tmp,dir_path);
																kstrcpy(path, tmp); 
}

int insertFDEntry(FD* file_d )
{
int i;
																for (i = 3; i < MAXFD; ++i) {
																								if (CURRENT_TASK->file_descp[i] == NULL) {
																																CURRENT_TASK->file_descp[i] = (uint64_t *)file_d;
																																return i;        
																								}
																}
return 0;
}
int createNewFile(fnode_t* temp_node,int* inode_no,ext_inode* inode_entry,char* dir_path,FD* file_d,fnode_t*currnode,uint64_t addr,uint64_t flags,char*temp)
{
																								temp_node = (fnode_t *)kmalloc(sizeof(fnode_t));                

																								*inode_no = alloc_new_inode();

																								if (*inode_no == -1) {
																																return -1;
																								}
																								kstrcpyn(inode_entry->i_name, (char*)dir_path, sizeof(inode_entry->i_name)-1);
																								inode_entry->i_size = 0;
																								inode_entry->i_block_count = 0;
																								write_inode(inode_entry, *inode_no);

																								file_d->inode_struct = (uint64_t) inode_entry;

																								//add as child of currnode 
																								make_node(temp_node, currnode, temp, 0, 0, FILE, *inode_no);  

																								currnode->f_child[currnode->end] = temp_node;
																								currnode->end += 1; 

																								file_d->filenode = temp_node;
																								file_d->curr     = addr;
																								file_d->f_perm   = flags;

																								//traverse file descriptor array and insert this entry
																								int i;
																								for (i = 3; i < MAXFD; ++i) {
																																if (CURRENT_TASK->file_descp[i] == NULL) {
																																								CURRENT_TASK->file_descp[i] = (uint64_t *)file_d;
																																								vmalogic(addr, 0, RW, FILETYPE, i);
																																								return i;        
																																}
																								}
return -1;
}
int sys_open(char* dir_path, uint64_t flags)
{
								fnode_t *temp_node;
								FD* file_d = (FD *)kmalloc(sizeof(FD));
								fnode_t *aux_node = NULL, *currnode = root_node;
								char tmp[256];
								char *path = (char *)kmalloc(256);
								if(dir_path[0]!='/'){
													handleNonRootCase(tmp,path,dir_path);
								}else{
																kstrcpy(path, dir_path); 
								}
								vma_struct *iter;
								uint64_t addr;
								char *temp = NULL; 
								int i, inode_no, creat_flag = 0;

								temp = kstrtok(path, "/");  

								if (temp == NULL) return -1;

								if (kstrcmp(temp, "rootfs") == 0) {
																	int j=0;
																for (;temp != NULL;j++) {
																								aux_node = currnode;

																								i=2;
																								while(i < currnode->end) {
																																if (kstrcmp(temp, currnode->f_child[i]->f_name) == 0) {
																																								currnode = (fnode_t *)currnode->f_child[i];
																																								break;       
																																}  
																														i++;      
																								}

																								if (i == aux_node->end) {
																																return -1;
																								}

																								temp = kstrtok(NULL, "/");          
																}

																if (currnode->f_type == DIRECTORY) {

																								return -1;

																}

																file_d->filenode = currnode;
																file_d->curr     = currnode->start;
																file_d->f_perm   = flags;

																int ret =insertFDEntry(file_d);
																if(ret)
																								return ret;

																return -1;

																} else if ( kstrcmp(temp, "Disk") == 0) {
																	int j =0;
															for (;temp != NULL;j++) {
																								aux_node = currnode;
																								i=2;
																								while ( i < currnode->end) {
																																if (kstrcmp(temp, currnode->f_child[i]->f_name) == 0) {
																																								currnode = (fnode_t *)currnode->f_child[i];
																																								break;       
																																}        
																										i++;
																								}

																								if (i == aux_node->end && flags == O_CREAT) {
																																creat_flag = 1; 
																																break; 
																								} else if (i == aux_node->end && flags != O_CREAT) {

																																return -1;
																								}

																								temp = kstrtok(NULL, "/");          
																}

																for (iter = CURRENT_TASK->mm->vma_list; iter->vm_next != NULL; iter = iter->vm_next);
																addr = (uint64_t)((((iter->vm_end - 1) >> 12) + 1) << 12);

																ext_inode* inode_entry = kmalloc(sizeof(ext_inode));                
																if (creat_flag == 1) { 
																							temp_node = NULL;
																			int ret=createNewFile(temp_node,&inode_no,inode_entry,dir_path,file_d,currnode,addr,flags,temp);
   
																			if(ret!=-1)
																			return ret;

																} else if (aux_node != currnode) {

																								read_inode(inode_entry, currnode->f_inode_no);

																								if (flags == O_TRUNC) {
																																truncate_inode(inode_entry, currnode->f_inode_no);
																								}

																								file_d->inode_struct = (uint64_t) inode_entry; 

																								file_d->filenode = currnode;
																								file_d->f_perm   = flags;

																								for (i = 3; i < MAXFD; ++i) {
																																if (CURRENT_TASK->file_descp[i] == NULL) {
																																								CURRENT_TASK->file_descp[i] = (uint64_t *)file_d;

																																								vma_struct *new_vma = vmalogic(addr, inode_entry->i_size, RW, FILETYPE, i);
																																								kmmap(new_vma->vm_start, new_vma->vm_end - new_vma->vm_start, RW_USER_FLAGS);
																																								copy_blocks_to_vma(inode_entry, new_vma->vm_start);

																																								if (flags == O_APPEND) {
																																																file_d->curr     = addr + inode_entry->i_size;
																																								} else {
																																																file_d->curr     = addr;
																																								}

																																								return i;        
																																}
																								}
																}
																return -1;
								}
								return -1;
}

void sys_close(int fd)
{
								ext_inode *inode_t = (ext_inode*) ((FD*) CURRENT_TASK->file_descp[fd])->inode_struct;
								int32_t inode_no = ((FD*) CURRENT_TASK->file_descp[fd])->filenode->f_inode_no;

								if (inode_t != NULL) {
																vma_struct *vma_l = CURRENT_TASK->mm->vma_list;
																vma_struct *last_vma_l = NULL;

																while (vma_l != NULL) {
																								if (vma_l->vm_file_descp == fd) {
																																break;
																								}
																								last_vma_l = vma_l;
																 vma_l = vma_l->vm_next;
																		}
																copy_vma_to_blocks(inode_t, inode_no, vma_l->vm_start, vma_l->vm_end - vma_l->vm_start);

																last_vma_l->vm_next = vma_l->vm_next; 
																add_to_vma_free_list(vma_l);
								}

								CURRENT_TASK->file_descp[fd] = NULL;
}

int sys_read(uint64_t fd_type, uint64_t addr, uint64_t length)
{
								uint64_t end = 0, currlength = 0;
								if (fd_type == STDIN) {
																length = gets(addr);
								} else if(fd_type > 2) {
																if ((CURRENT_TASK->file_descp[fd_type]) == NULL)
																								length = -1;
															 else if(((FD *)CURRENT_TASK->file_descp[fd_type])->f_perm == O_WRONLY )																								length = -1; 
																else if(((FD *)CURRENT_TASK->file_descp[fd_type])->filenode->f_inode_no != 0) { 
																								vma_struct *iter; 
																								currlength = (uint64_t)((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr;
																							iter = CURRENT_TASK->mm->vma_list;
																								while (iter != NULL ) {
																																if(iter->vm_file_descp == fd_type){
																																								end   = iter->vm_end;
																																								break; 
																																}
																										iter = iter->vm_next;
																								}

																} else {

																								currlength = (uint64_t)((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr;
																								end        = ((FD *)(CURRENT_TASK->file_descp[fd_type]))->filenode->end;
																}

																if ((end - currlength) < length) length = (end - currlength);
																memcpy((void *)addr, (void *)currlength, length);
																((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr += length;
								}
								return length;
}

int sys_write(uint64_t fd_type, uint64_t addr, int length)
{

								if (fd_type == STDOUT || fd_type ==STDERR) {
																length = 0; 
																length = puts((char*) addr);

								} else if (fd_type > 2) {
																vma_struct *iter; 

																if ((CURRENT_TASK->file_descp[fd_type]) == NULL)
																								length = -1;
																else if(((FD *)CURRENT_TASK->file_descp[fd_type])->f_perm == O_RDONLY )																								length = -1; 
																else {
																								uint64_t end = 0, currlength = 0;

																								currlength = ((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr;
																							iter = CURRENT_TASK->mm->vma_list;
																								while (iter != NULL ) {
																						//		for (iter = CURRENT_TASK->mm->vma_list; iter != NULL; iter = iter->vm_next) {
																																if(iter->vm_file_descp == fd_type){
																																								end = iter->vm_end;
																																								break; 
																																}
																										iter = iter->vm_next;
																								}

																								if (currlength + length > end) {
																																kmmap(iter->vm_end, (currlength + length) - iter->vm_end, RW_USER_FLAGS);
																																iter->vm_end = currlength + length;
																								}

																								memcpy((void *)currlength, (void *)addr, length);
																								((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr += length;
																} 
								}
								return length;
}

int sys_lseek(uint64_t fd_type, int offset, int whence) 
{


								vma_struct* iter;
								ext_inode *inode_t = (ext_inode*) ((FD*) CURRENT_TASK->file_descp[fd_type])->inode_struct;
								uint64_t start = 0, end = 0;    
								if (((FD*) CURRENT_TASK->file_descp[fd_type])->filenode->f_type == DIRECTORY) {
																offset = -1;

								} else {
																if (inode_t == NULL) {
																								start = ((FD*) CURRENT_TASK->file_descp[fd_type])->filenode->start;
																								end   = ((FD*) CURRENT_TASK->file_descp[fd_type])->filenode->end;


																} else {
																							iter = CURRENT_TASK->mm->vma_list;
																								while (iter != NULL ) {

																							//	for (iter = CURRENT_TASK->mm->vma_list; iter != NULL; iter = iter->vm_next) {
																																if(iter->vm_file_descp == fd_type)
																																								break; 
																										iter=iter->vm_next;
																								}

																								start = iter->vm_start; 
																								end   = iter->vm_end; 

																}

																if(whence == SEEK_SET) {
																								if (offset < 0)
																																offset = 0;

																								((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr = start + offset;

																} else if(whence == SEEK_CUR) {

																								if(((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr + offset > end) {

																																((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr = end;
																								} else {        

																																((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr += offset;
																								}

																} else if(whence == SEEK_END) {

																								if (offset > 0)
																																offset = 0;

																								((FD *)(CURRENT_TASK->file_descp[fd_type]))->curr = end + offset;


																}else
																								offset = -1;
								} 
								return offset;

}

pid_t sys_fork()
{
								task_struct *parent_task = CURRENT_TASK; 
								task_struct* child_task = copy_task_struct(parent_task); 
								schedule_process(child_task, parent_task->kernel_stack[KERNEL_STACK_SIZE-6], parent_task->kernel_stack[KERNEL_STACK_SIZE-3]);
								child_task->kernel_stack[KERNEL_STACK_SIZE-7] = 0UL;
								return child_task->pid;
}

int sys_killProcess(pid_t pid)
{
								task_struct *cur_task = CURRENT_TASK;
								int flag = 0;
								task_struct *init_task= NULL;
								while(cur_task)
								{
																if(cur_task->pid == 1)
																{
																								init_task = cur_task;
																								break;
																}
																cur_task=cur_task->next;
								}
								cur_task = CURRENT_TASK;
								while(cur_task)
								{
																if(cur_task->pid == pid){
																								flag = 1;
																								break;
																}
																cur_task=cur_task->next;
								}

								if(flag==0)
								{
																kprintf("PID invalid\n");
																return -1;	
								}
								else
								{
																//								empty_task_struct(cur_task);
																cur_task->task_state = EXIT_STATE;
																int ppid = cur_task->ppid;
																if(cur_task->no_children != 0)
																{
																								task_struct* child=cur_task->childhead;
																								while(child)
																								{
																																if(child->pid==1)
																																								break;
																																child->ppid =  1;
																																child->parent=init_task;
																																child=child->next;      
																								}
																}
																//TODO look at this again
																task_struct *t = CURRENT_TASK;
																while(t){
																								if(t->pid == ppid){
																																t->task_state = READY_STATE;
																																break;
																								}
																								t=t->next;
																}
																return 0;
								}
								return -1;
}

char* sys_getcwd(char *buf, int size){
								if(kstrlen(CURRENT_TASK->cwd) >= size){
																kprintf("Size of buffer is not enough for the cwd path\n");
																return NULL;
								}
								if(size==0 && buf !=NULL){
																kprintf("Size of buffer is zero");
																return NULL;
								}
								kstrcpy(buf,CURRENT_TASK->cwd);
								return buf;
}

int sys_chdir(const char *path){
								if(kstrlen(path) >= 256){
																kprintf("Size of path is greater than max size(256) allowd\n");
																return -1;
								}
								if(path ==NULL){
																kprintf("Path is empty");
																return -1;
								}
								kstrcpy(CURRENT_TASK->cwd, path);
								return 0;

}

uint64_t sys_execvpe(char *file, char *argv[], char *envp[])
{
								task_struct *new_task = create_elf_proc(file, argv);
								if (new_task) {
																task_struct *cur_task = CURRENT_TASK;
																set_next_pid(new_task->pid);
																new_task->pid  = cur_task->pid;
																new_task->ppid = cur_task->ppid;
																new_task->parent = cur_task->parent;
																kstrcpy(new_task->cwd, cur_task->cwd);
																memcpy((void*)new_task->file_descp, (void*)cur_task->file_descp, MAXFD*8);
																replace_child_task(cur_task, new_task);
																empty_task_struct(cur_task);
																cur_task->task_state = EXIT_STATE;
																__asm__ __volatile__ ("int $32");
								}
								return -1;
}

uint64_t sys_wait(uint64_t status)
{
								volatile task_struct *cur_task = CURRENT_TASK;
								int *status_p = (int*) status;

								if (cur_task->no_children == 0) {
																if (status_p) *status_p = -1;
																return -1;
								}

								cur_task->wait_on_child_pid = 0;
								cur_task->task_state = WAIT_STATE;

								__asm__ __volatile__ ("int $32");

								if (status_p) *status_p = 0;
								return (uint64_t)cur_task->wait_on_child_pid;
}

uint64_t sys_waitpid(uint64_t fpid, uint64_t fstatus, uint64_t foptions)
{
								pid_t pid = fpid;
								volatile task_struct *cur_task = CURRENT_TASK;
								int *status_p = (int*) fstatus;

								if (cur_task->no_children == 0) {
																if (status_p) *status_p = -1;
																return -1;
								}

								if (pid > 0)
																cur_task->wait_on_child_pid = pid;
								else
																cur_task->wait_on_child_pid = 0;
								
								cur_task->task_state = WAIT_STATE;

								__asm__ __volatile__ ("int $32");

								if (status_p) *status_p = 0;
								return (uint64_t)cur_task->wait_on_child_pid;
}

void sys_exit()
{
								task_struct *cur_task = CURRENT_TASK;

								if (cur_task->parent){
																task_struct *parent_task = cur_task->parent;
																task_struct *sibling_l, *last_sibling;

																sibling_l = parent_task->childhead;
																last_sibling = NULL;
																while (sibling_l) {
																								if (sibling_l == cur_task) {
																																break;
																								}
																								last_sibling = sibling_l;
																								sibling_l = sibling_l->siblings;
																}

																if (!sibling_l) return; 

																if (last_sibling) {
																								last_sibling->siblings = sibling_l->siblings;    
																} else {
																								parent_task->childhead = sibling_l->siblings;
																}

																parent_task->no_children--;
																if (parent_task->task_state == WAIT_STATE) {
																								if (!parent_task->wait_on_child_pid || parent_task->wait_on_child_pid == cur_task->pid) {
																																parent_task->wait_on_child_pid = cur_task->pid;
																																parent_task->task_state = READY_STATE;
																								}
																}

								}

								if (cur_task->childhead){
																task_struct *child_next = cur_task->childhead;
																task_struct *child_cur = NULL;

																while (child_next) {
																								child_cur = child_next;
																								child_cur->task_state = ZOMBIE_STATE;
																								child_next = child_next->siblings;
																								child_cur->siblings = NULL;
																}
								}

								empty_task_struct(cur_task);
								cur_task->task_state = EXIT_STATE;

								__asm__ __volatile__ ("int $32");
}

int sys_sleep(int msec)
{
								task_struct *task = CURRENT_TASK;

								task->sleep_time = msec/10;
								task->task_state = SLEEP_STATE;
								__asm__ __volatile__("int $32;");

								return task->sleep_time;
}

uint64_t sys_brk(uint64_t no_of_pages)
{
								uint64_t new_vaddr = CURRENT_TASK->mm->end_brk;

								increment_brk(CURRENT_TASK, PAGESIZE * no_of_pages);

								return new_vaddr;
}

uint64_t sys_mmap(uint64_t addr, uint64_t nbytes, uint64_t flags)
{
								vma_struct *iter;    

								if (addr == 0x0) {
																for (iter = CURRENT_TASK->mm->vma_list; iter->vm_next != NULL; iter = iter->vm_next);

																addr = (uint64_t)((((iter->vm_end - 1) >> 12) + 1) << 12);

								} else if (verify_addr(CURRENT_TASK, addr, addr + nbytes) == 0) {
																return NULL;
								} 

								vmalogic(addr, nbytes, RW, ANON, 0);
								return addr;
}

pid_t sys_getpid()
{
								return CURRENT_TASK->pid;
}

pid_t sys_getppid()
{
								return CURRENT_TASK->ppid;
}

int sys_clear()
{
								clear_screen();
								return 1;
}

void sys_yield()
{
								__asm__ __volatile__ ("int $32");
}

const char *t_state[NUM_TASK_STATES] = { "RUNNING" , "READY  " , "SLEEP  " , "WAIT   " , "IDLE   " , "EXIT   ", "ZOMBIE "};

void sys_listprocess()
{
								int i = 0;
								task_struct *cur = CURRENT_TASK;

								kprintf("\n ===== PROCESSES LIST ====== "
																								"\n  #  |  PID  |  PPID  |   State   |  Process Name "
																								"\n ----| ----- | ------ | --------- | --------------- ");

								while (cur) {
																kprintf("\n  %d  |   %d   |   %d    |  %s  |  %s  ",
																																++i, cur->pid, cur->ppid, t_state[cur->task_state], cur->comm);
																cur = cur->next;
								}
}    

void sys_shutdown()
{
								task_struct *cur = CURRENT_TASK;
								while (cur) {
																cur->task_state = EXIT_STATE; 
																cur = cur->next;
								}

								sys_clear();
								kprintf("\n=========== System  is shuting down!!!!!!!!!!!===============");
								while(1);    
}

void* syscall_tbl[NUM_SYSCALLS] = 
{
								sys_read,
								sys_write,
								sys_brk,
								sys_fork,
								sys_execvpe,
								sys_wait,
								sys_waitpid,
								sys_exit,
								sys_yield,
								sys_mmap,
								sys_getpid,
								sys_getppid,
								sys_listprocess,
								sys_opendir,
								sys_readdir,
								sys_closedir, 
								sys_open,
								sys_close,
								sys_sleep,
								sys_clear,
								sys_lseek,
								sys_mkdir,
								sys_shutdown,
								sys_killProcess,
								sys_getcwd,
								sys_chdir
};

// The handler for the int 0x80
void syscall_handler(void)
{
								uint64_t syscallNo;
								__asm__ __volatile__("movq %%rax, %0;" : "=r"(syscallNo));

								if (syscallNo >= 0 && syscallNo < NUM_SYSCALLS) {
																void *func_ptr;
																uint64_t ret;

																__asm__ __volatile__("pushq %rdx;");
																func_ptr = syscall_tbl[syscallNo];
																__asm__ __volatile__(
																																"movq %%rax, %0;"
																																"popq %%rdx;"
																																"callq *%%rax;"
																																: "=a" (ret) : "r" (func_ptr)
																																);
								}
								__asm__ __volatile__("iretq;");
}

