#include <sys/defs.h>
#include <sys/common.h>

int kstrcmp(const char *s1, const char *s2)
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


int kstrlen(const char *str)
{
    int len=0;
    while (*str++ != '\0')
        len += 1;
    return len;
}

char* kstrcat(char *str1, const char *str2)
{
    uint64_t len1 = kstrlen(str1);
    uint64_t len2 = kstrlen(str2);
    uint64_t i = 0;

    for(i = 0; i < len2 ; i++)
        str1[len1 + i] = str2[i];
    str1[len1 + i] = '\0';

    return str1;    
}

char * kstrcpy(char *dest, const char *src)
{
    char *str = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return str;
}

char * kstrcpyn(char *destination, const char *source, uint64_t n)
{
    uint64_t i = 0;
    char *str = destination;

    for (i = 0; i < n; i++) {
        *destination++ = *source++;
        if ( *source == '\0') {
            *destination++ = '\0';
            return(str);
        }
    }
    *destination++ = '\0';

    return str;
}

char * kstrtok(char *s, const char *delim)
{
    static char *last=NULL;
    char *spanp;
    int c, sc;
    char *tok;


    if (s == NULL && (s = last) == NULL)
        return (NULL);

t:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto t;
    }

    if (c == 0) {      
        last = NULL;
        return (NULL);
    }
    tok = s - 1;

    while (1) {
        c = *s++;
        spanp = (char *)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                last = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

