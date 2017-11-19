#include <sys/util.h>
#include<sys/defs.h>
#include<sys/kprintf.h>

void outb (unsigned short port, unsigned char data){
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

unsigned char inb (unsigned short port){
    unsigned char ret;
	__asm__ __volatile__ ("inb %1, %0" :"=a"(ret) :"Nd" (port));
    return ret;
}
#define __NR_read 0
#define __NR_open 2


void copyHelper(char* src, char*dest, int offset, int len){
	if(src == NULL || dest == NULL) return;
	int i=offset;
	int j=0;
	for(; i< len; i++){
		dest[j++] = src[i];
	}
	dest[j] = '\0';
}
int cStrlen(char *str){
	int i=0;
	for(; str[i]!='\0';i++);
	return i;
}
int open(const char *pathname, int flags){
    int  n;
    __asm__ __volatile__
    (
        "syscall"
        : "=a" (n)
        : "0"(__NR_open), "D"(pathname), "S"(flags)
        : "cc", "rcx", "r11", "memory"
    );
    return n;
}

int read(int fd, void *buf, int count){
    int n;
    __asm__ __volatile__
    (
        "syscall"
        : "=a" (n)
        : "0"(__NR_read), "D"(fd), "S"(buf), "d"(count)
        : "cc", "rcx", "r11", "memory"
    );
    return n;
}

int32_t atoi(char * ptr)
{
int x = 0;
int sign = 1;
if(ptr[0] == '-')
sign =-1;
int i =1;
for(;ptr[i]!='\0';i++)
{
x*=10;
x+=(ptr[i] - '0');
}

return x*sign;
}


int32_t oct_to_dec(int n) {
    int dec = 0, i = 0, rem; 
int k =1; 
    while (n != 0) { 
        rem = n % 10; 
        n /= 10;
								if(i>=1)
								k*=8; 
        dec += rem * k;
        ++i;
    }
    return dec;
}

int strfind(const char* s1, const char* s2){
        const char *p = s1, *s;
	char c, sc;
	while(1) {
		c = *p++;
		s = s2;
		do {
			if ((sc = *s++) == c)
				return (p - 1 - s1);
		} while (sc != 0);
	}
}

char *strchr(const char *s, int c){
	const char ch = c;
	for ( ; *s != ch; s++)
		if (*s == '\0')
			return 0;
	return (char *)s;
}

char * strtok(char *s, const char* delim){
	static char *l;
	int ch;

	if (s == 0)
		s = l;
	do {
		if ((ch = *s++) == '\0')
			return 0;
	} while (strchr(delim, ch));
	--s;
	l = s + strfind(s, delim);
	if (*l != 0)
		*l++ = 0;
	return s;
}


int cStrCmp(char *str1, char* str2){
	int i = 0;
	for(; str1[i] != '\0' && str2[i] != '\0'; i++){
		if(str1[i] != str2[i]) return -1;
	}
	if(str1[i] != str2[i]) return -1;
	return 0;
}

void memset(char *str, char val, int size)
{
int i =0;
for(i=0;i<size;i++){
    str[i] = val;
}
}
 void *memcpy(void *destination, void *source, uint64_t num) 
{
    uint8_t *dest = (uint8_t *)destination;
    uint8_t *src = (uint8_t *)source;

    while(num--) {
        *dest++ = *src++; 
    }

    return destination;
}
