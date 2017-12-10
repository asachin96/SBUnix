#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>
#include <unistd.h>

//int main(int argc, char *argv[], char *envp[]);
int main(int argc, char *argv[]);
void exit(int status);
int printf(const char *format, ...);
void scanf(const char *format, ...);
int cls();
int kill(pid_t pid);
void *malloc(int size);
void free(void *ptr);
void shutdown();

void* mmap(void* addr, uint64_t nbytes, uint64_t flags);
int munmap(uint64_t* addr, uint64_t size);

int seek(uint64_t file_d, int offset, int whence);

pid_t getpid();
pid_t getppid();
void listprocess();

pid_t fork();
int execvpe(const char *file, char *const argv[], char *const envp[]);
void yield();

void *memcpy(void *destination, void *source, uint64_t num);
void *memset(void *ptr, uint8_t value, uint64_t num);
char * strtok(char *s, const char *delim);
int strcmp(const char *s1, const char *s2);
int strlen(const char *str);
char* strcat(char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
int32_t pow(int base, int power);
int32_t oct_to_dec(int n);
int32_t atoi(char *p);
char *itoa(uint64_t val, char *str, int32_t base);

#endif
