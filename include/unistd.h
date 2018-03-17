#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/defs.h>

int open(char *pathname, int flags);
void close(int fd);
uint64_t read(uint64_t fd, void *buf, uint64_t count);
int write(int fd, char *buf, int count);
int unlink(const char *pathname);

int chdir(const char *path);
char *getcwd(char *buf, size_t size);

pid_t fork();
int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

int sleep(int seconds);

pid_t getpid(void);
pid_t getppid(void);

// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
int mkdir(char *pathname);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);

#endif
