#include <stdlib.h>

void *memset(void *ptr, uint8_t value, uint64_t num)
{
    uint8_t *temp = (uint8_t *)ptr;

    while(num--) {
        *temp++ = value; 
    }
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
    int len=0;
    while (*str++ != '\0')
        len += 1;
    return len;
}

char* strcat(char *str1, const char *str2)
{
								uint64_t len1 = strlen(str1);
								uint64_t len2 = strlen(str2);
								uint64_t i = 0;

								while(i<len2)
								{
																str1[len1 + i] = str2[i];
																i++;
								}
								str1[len1 + i] = '\0';

								return str1;    
}

char *strcpy(char *dest, const char *src)
{
    char *str = dest;
    while (*src !='\0')
    {
        *dest++ = *src++;
    }
    *dest = '\0';
    return str;
}

// Copies 1 byte at a time
void *memcpy(void *destination, void *source, uint64_t num) 
{
    uint8_t *dest = (uint8_t *)destination;
    uint8_t *src = (uint8_t *)source;
int i = num;
				for(;i>0;i--)   {
        *dest++ = *src++; 
    }

    return destination;
}

int32_t pow(int base, int power)
{ 
    int i = 0, product = 1;

  //  for(i = 0; i < power; ++i) {
							while(i<power)
						{
        product = product * base;
								i++;
    }

    return product;
}

int32_t atoi(char *str){
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


int32_t oct_to_dec(int n) {
    int dec = 0, i = 0, rem; 
 
    for(;n!=0;i++){ 
        rem = n % 10; 
        n /= 10; 
        dec += rem * pow(8, i);
    }
    return dec;
}

char *itoa(uint64_t val, char *str, int32_t base)
{
    *str = '\0'; // Currently pointing to the end of string
				const char*  hexString ="0123456789abcdef";
    if (val == 0) {
        *--str = '0';
        return str;
    }
    if (base != 10 && base != 16) {
        return str;
    }
    while (val) {
        *--str = hexString[val%base];
        val = val/base;
    }
    return str;
}
