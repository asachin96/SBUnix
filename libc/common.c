#include <sys/defs.h>
#include <syscall.h>
#include <stdlib.h>

pid_t getpid()
{
    return __syscall0(GETPID);
}

pid_t getppid()
{
    return __syscall0(GETPPID);
}

int kill(pid_t pid)
{
				return __syscall1(KILL, (uint64_t) pid);
}

pid_t fork()
{
    return __syscall0(FORK);
}

int chdir(const char *path){
    return __syscall1(CHDIR, (uint64_t)path);
}

char *getcwd(char *buf, int size){
    return (char*)__syscall2(GETCWD, (uint64_t)buf, (uint64_t)size);
}

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    return __syscall3(EXECVPE, (uint64_t)file, (uint64_t)argv, (uint64_t)envp);
}

pid_t wait(int *status)
{
    return __syscall1(WAIT, (uint64_t)status);
}

pid_t waitpid(pid_t pid, int *status, int options)
{
    return __syscall3(WAITPID, (uint64_t)pid, (uint64_t)status, (uint64_t)options);
}

void listprocess()
{
    __syscall0(LISTPROCESS);
}

void yield()
{
    __syscall0(YIELD);
}

void shutdown()
{
    __syscall0(SHUTDOWN);
}


