#ifndef __IDT_H__
#define __IDT_H__

#include <sys/defs.h>

void setInteruptHandler(int intruptNo, uint64_t handler); 
void installIdt(); 

#endif
