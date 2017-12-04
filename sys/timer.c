#include <sys/timer.h>
#include <sys/util.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
extern void printString(char *); 
int noOfSecs = 0;
static char *timerLocation=(char*)0xffffffff800b8000 + (160*25-14*2);

void printInteger(int n){
		int a[10];
		int j=0;
		while(n){
				a[j++] = n%10;
				n = n/10;
		}
		j--;
		if(j == 0){
				*timerLocation = '0';
				timerLocation += 2;
		}    
		while(j>=0){
				char c = a[j] + 48;
				j--;
				*timerLocation = c;
				timerLocation += 2;
		}
}

void printStr(char *s){
		while(*s){
				*timerLocation = *s;
				timerLocation += 2;
				s++;
		}
}

void initTimer(){
		uint32_t divisor =  59659;

		//send the command byte
		outb(0x43, 0x36);

		//send the divisor in parts
		outb(0x40, divisor & 0xFF);
		outb(0x40, divisor >> 8);
}

void displayTime(){
  noOfSecs++;
		int secs = noOfSecs%60;
		int mins = noOfSecs/60;
		int hrs = noOfSecs/3600;
		(hrs)? printInteger(hrs) : printStr("00");    
		*timerLocation = ':';
		timerLocation += 2;
		(mins)? printInteger(mins) : printStr("00");    
		*timerLocation = ':';
		timerLocation += 2;
		printInteger(secs);    
		timerLocation=(char*)0xffffffff800b8000 + (160*25 - 14*2);
}
