#include <stdlib.h>

void *memset(void *ptr, uint8_t value, uint64_t num)
{
								uint8_t* t=ptr;
								int i=0;
								for(;i<num;i++) *t++=value;
								return ptr;
}


int strcmp(const char *s1, const char *s2)
{
								int i = 0;
								for(;s1[i]!='\0'&&s2[i]!='\0';i++)
								{
																if(s1[i]!=s2[i])
																								return (s2[i]>s1[i])?-1:1;
								}
								if(s1[i]==s2[i])
																return 0;
								else if(s1[i] == '\0')
																return -1;
								else 
																return 1;
}


int strlen(const char *str)
{
								int len = 0;
								while(*str++) len++;
								return len;
}

char* strcat(char *str1, const char *str2)
{
								int len = strlen(str1);
								char *t = str1;
								while(*str2){ 
																*(str1+len) = *str2;
																str1++;
																str2++;
								}
								return t;
} 

char *strcpy(char *dest, const char *src)
{
								memset(dest, 0, strlen(dest));
								strcat(dest,src);
								return dest;
}

void *memcpy(void *dest, void *src, uint64_t num) 
{
								while(num--)
																*((uint8_t*)dest++) = *((uint8_t*)src++);
								return dest;
}

int pow(int base, int power)
{ 
								int res = 1;
								while(power--) res=res*base;
								return res;
}

int atoi(char *str){
								int res = 0;  
								int sign = 1; 
								int i = 0;  

								// If number is negative, then update sign
								if (str[0] == '-')
								{
																sign = -1;  
																i++; 
								}

								// Iterate through all digits and update the result
								for (; str[i] != '\0'; ++i)
																res = res*10 + str[i] - '0';

								// Return result with sign
								return sign*res;
}


int oct_to_dec(int n) {
								int dec = 0, i = 0, rem; 

								for(;n!=0;i++){ 
																rem = n % 10; 
																n /= 10; 
																dec += rem * pow(8, i);
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

int readline(int fd, char buf[]){
								char c;
								int i=0, ret;
								ret = read(fd, &c, 1);
								while(c!='\n'&&ret>0){
																buf[i++] = c;
																ret = read(fd, &c, 1);
								}   
        buf[i] = '\0';
        return ret;
}
