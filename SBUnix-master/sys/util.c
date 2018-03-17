#include <sys/util.h>
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/memoryMgr.h>
extern uint64_t nextAddr;
void outw (uint16_t port, uint16_t data){
	__asm__ volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

void outb (unsigned short port, unsigned char data){
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

unsigned char inb (unsigned short port){
	unsigned char ret;
	__asm__ __volatile__ ("inb %1, %0" :"=a"(ret) :"Nd" (port));
	return ret;
}

int atoi(char *str){
	if(!str) return 0;
	int res = 0;  
	int sign = 1; 
	int i = 0;  

	if (str[0] == '-'){
		sign = -1;  
		i++; 
	}

	for (; str[i] != '\0'; ++i){
		int t = str[i] - '0';
		if(t>9 && t<0) return 1;
		res = res*10 + t;
	}

	return sign*res;
}

int getNoPages(uint64_t start, int size){
	return ((start+size-1)/4096-start/4096+1);
}

int toDecimal(int n){
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

void memset(char *str, char val, int size){
	int i =0;
	for(i=0;i<size;i++){
		str[i] = val;
	}
}

void *memcpy(void *destination, void *source, int num){
	unsigned char *p = (unsigned char *)destination;
	unsigned char *q = (unsigned char *)source;
	for(int i=0;i<num;i++,p++,q++) {
		*p = *q; 
	}
	return destination;
}

static int free;
static uint64_t currptr;

void* kmalloc(int size){
	if(size>free){
		int pages = size/4096;     
		uint64_t temp = nextAddr;
		allocatePages(&nextAddr, PAGE_P, pages+1); 
		free = ((pages+1) * 4096) - size;
		currptr = temp + size;
		return (void*)temp;
	}
	void* ret = (void*)currptr;
	currptr += size;
	free += -size;
	return ret;
}

int kstrcmp(const char *str1, const char *str2){
	int i = 0;
	for(;str1[i]!='\0'&&str2[i]!='\0';i++){
		if(str1[i]!=str2[i])
			return 1;
	}
	if(str1[i]==str2[i]) return 0;
	return 1;
}

int kstrfind(const char* s1, const char* s2){
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

char *kstrchr(const char *s, int c){
	const char ch = c;
	for ( ; *s != ch; s++)
		if (*s == '\0')
			return 0;
		return (char *)s;
}

int kstrlen(const char *str){
	int len=0;
	while (*str++ != '\0')
		len += 1;
	return len;
}

char* kstrcat(char *str1, const char *str2){
	int len = kstrlen(str1);
	char *t = str1;
	while(*str2){
		*(str1+len) = *str2;
		str1++;
		str2++;
	}
	return t;
}

char* kstrcpy(char *dest, const char *src){
	dest[0] = '\0';
	kstrcat(dest, src);
	return dest;
}

char* kstrtok(char *s, const char *delim){
	static char *l;
	int ch;
	if (s == 0)
		s = l;
	do {
		if ((ch = *s++) == '\0')
			return 0;
	} while (kstrchr(delim, ch));
	--s;
	l = s + kstrfind(s, delim);
	if (*l != 0)
		*l++ = 0;
	return s;
}

