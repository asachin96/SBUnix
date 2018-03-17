#include <sys/defs.h>
#include <syscall.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>

int open(char *path, int flags)
{
    uint64_t ret;
    __asm__ __volatile__(
        "movq %[path], %%rdi;" 
        "movq %[flags], %%rsi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_open), [path]"D"((uint64_t)path), [flags]"S"((uint64_t)flags)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

char *brk(int size)
{
   char *ret;
    __asm__ __volatile__(
        "movq %[size], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_brk), [size]"D"((uint64_t)size)
        : "%rcx", "cc", "r11", "memory"
    );
   return ret;
}

void close(int fd)
{
    int ret;
    __asm__ __volatile__(
        "movq %[fd], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_close), [fd]"D"((uint64_t)fd)
        : "%rcx", "cc", "r11", "memory"
    );
}

int read(int fd, void *buf, int nbytes)
{
    int ret;
    __asm__ __volatile__(
        "movq %[fd], %%rdi;" 
        "movq %[buf], %%rsi;" 
        "movq %[nbytes], %%rdx;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_read), [fd]"D"((uint64_t)fd), [buf]"S"((uint64_t)buf), [nbytes]"r"((uint64_t)nbytes)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

int write(int fd, char *buf, int nbytes)
{
    int ret;
    __asm__ __volatile__(
        "movq %[fd], %%rdi;" 
        "movq %[buf], %%rsi;" 
        "movq %[nbytes], %%rdx;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_write), [fd]"D"((uint64_t)fd), [buf]"S"((uint64_t)buf), [nbytes]"r"((uint64_t)nbytes)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

int seek(int fd, int offset, int whence)
{ 
    int ret;
    __asm__ __volatile__(
        "movq %[fd], %%rdi;" 
        "movq %[offset], %%rsi;" 
        "movq %[whence], %%rdx;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_seek), [fd]"D"((uint64_t)fd), [offset]"S"((uint64_t)offset), [whence]"r"((uint64_t)whence)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}


DIR* opendir(char *dir_path)
{
    DIR *dir = (DIR *)malloc(sizeof(DIR));
    char *path = (char*)malloc(sizeof(strlen(dir_path)));
    strcpy(path, dir_path);
    DIR *ret = NULL;
    __asm__ __volatile__(
        "movq %[dir_path], %%rdi;" 
        "movq %[dir], %%rsi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_opendir), [dir_path]"D"((uint64_t)path), [dir]"S"((uint64_t)dir)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret;
}

struct dirent* readdir(DIR* node)
{   
    struct dirent* curr = NULL; 
    __asm__ __volatile__(
        "movq %[node], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (curr)
        : [no]"rax"((uint64_t)__NR_readdir), [node]"D"((uint64_t)node)
        : "%rcx", "cc", "r11", "memory"
    );
    return curr;
}

int closedir(DIR* node)
{
    int ret; 
    __asm__ __volatile__(
        "movq %[node], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_closedir), [node]"D"((uint64_t)node)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret;
}

pid_t getpid()
{
    pid_t ret;
    __asm__ __volatile__(
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_getpid)
        : "%rcx"
    );
    return ret; 
}

pid_t getppid()
{
    pid_t ret;
    __asm__ __volatile__(
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_getppid)
        : "%rcx"
    );
    return ret; 
}

int kill(pid_t pid, int sig)
{
    int ret;
    __asm__ __volatile__(
        "movq %[pid], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_kill), [pid]"D"((uint64_t)pid)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

pid_t fork()
{
    pid_t ret;
    __asm__ __volatile__(
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_fork)
        : "%rcx"
    );
    return ret; 
}

int chdir(const char *path){
    int ret;
    __asm__ __volatile__(
        "movq %[path], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_chdir), [path]"D"((uint64_t)path)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

char *getcwd(char *buf, int size){
    uint64_t ret;
    __asm__ __volatile__(
        "movq %[buf], %%rdi;" 
        "movq %[size], %%rsi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_getcwd), [buf]"D"((uint64_t)buf), [size]"S"((uint64_t)size)
        : "%rcx", "cc", "r11", "memory"
    );
    return (char*)ret; 
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    uint64_t ret;
    __asm__ __volatile__(
        "movq %[file], %%rdi;" 
        "movq %[argv], %%rsi;" 
        "movq %[envp], %%rdx;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_execvpe), [file]"D"((uint64_t)file), [argv]"S"((uint64_t)argv), [envp]"r"((uint64_t)envp)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

pid_t waitpid(pid_t pid, int *status, int options)
{
    pid_t ret;
    __asm__ __volatile__(
        "movq %[pid], %%rdi;" 
        "movq %[status], %%rsi;" 
        "movq %[options], %%rdx;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_waitpid), [pid]"D"((uint64_t)pid), [status]"S"((uint64_t)status), [options]"r"((uint64_t)options)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret; 
}

void listprocess()
{
    int ret;
    __asm__ __volatile__(
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_listprocess)
        : "%rcx"
    );
}

int clear()
{
   int ret;
    __asm__ __volatile__(
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_clear)
        : "%rcx"
    );
    return ret; 
}

void exit(int status)
{
    pid_t ret;
    __asm__ __volatile__(
        "movq %[status], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_exit), [status]"D"((uint64_t)status)
        : "%rcx", "cc", "r11", "memory"
    );
}

int sleep(int seconds)
{
      int ret;
    __asm__ __volatile__(
        "movq %[status], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_sleep), [status]"D"((uint64_t)seconds)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret;
}

sighandler_t signal(int signum, sighandler_t handler){
    uint64_t ret;
    __asm__ __volatile__(
        "movq %[signum], %%rdi;" 
        "movq %[handler], %%rsi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_signal), [signum]"D"((uint64_t)signum), [handler]"S"((uint64_t)handler)
        : "%rcx", "cc", "r11", "memory"
    );
    return handler; 
    
}

int pipe(int pipefd[2]){
    int ret;
    __asm__ __volatile__(
        "movq %[pipefd], %%rdi;" 
        "movq %[no], %%rax;"
        "int $0x80;"
        : "=a" (ret)
        : [no]"rax"((uint64_t)__NR_pipe), [pipefd]"D"((uint64_t)pipefd)
        : "%rcx", "cc", "r11", "memory"
    );
    return ret;

}
