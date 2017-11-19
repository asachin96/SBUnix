#ifndef _DEFS_H
#define _DEFS_H

#define NULL  0
#define TRUE  1
#define FALSE 0

#define stdin 0
#define stdout 1
#define stderr 2

typedef int size_t;

// Unsigned data types
typedef unsigned long   __uint64_t;
typedef unsigned int    __uint32_t;
typedef unsigned short  __uint16_t;
typedef unsigned char   __uint8_t;

typedef __uint64_t      uint64_t;
typedef __uint32_t      uint32_t;
typedef __uint16_t      uint16_t;
typedef __uint8_t       uint8_t;
typedef uint64_t uintptr_t;
// Signed data types
typedef long            __int64_t;
typedef int             __int32_t;
typedef short           __int16_t;
typedef char            __int8_t;

typedef __int64_t       int64_t;
typedef __int32_t       int32_t;
typedef __int16_t       int16_t;
typedef __int8_t        int8_t;

// MISC data types
typedef __uint8_t       bool;
typedef __int8_t        pid_t;
typedef __uint8_t       BYTE; 

#define ROUNDDOWN(a,n,b)\
do\
{ \
uint64_t d = (uint64_t)a; \
b = (__typeof__(a)) (d - d % (n)); \
}while(0);

#define ROUNDUP(a,n,b) \
do \
{ \
uint64_t f =(uint64_t) n; \
uint64_t res = 0; \
ROUNDDOWN((uint64_t)(a) + f-1,f,res) \
b = (__typeof__(a))(res); \
}while(0);	

#endif
