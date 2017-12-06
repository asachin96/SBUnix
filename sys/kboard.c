#include <sys/kBoard.h>
#include <sys/util.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
//static char *keyLocation=(char*)0xffffffff800b8000 + (160*25- 2*2);
extern char*temp;
extern int linePos;
//TODO backspace implementation
struct scan_lookup 
{
		uint32_t scan_code;
		char value;
		char shiftValue;
}sc;
int order = 0;
int shiftcount = 0;
int ctrlcount = 0;
volatile int gets_triggered;
int counter;
char buf[1024];
struct scan_lookup scan_lookup_array[] =
{
		{0x1E,'a','A'},
		{0x30,'b','B'},
		{0x2E,'c','C'},
		{0x20,'d','D'},
		{0x12,'e','E'},
		{0x21,'f','F'},
		{0x22,'g','G'},
		{0x23,'h','H'},
		{0x17,'i','I'},
		{0x24,'j','J'},
		{0x25,'k','K'},
		{0x26,'l','L'},
		{0x32,'m','M'},
		{0x31,'n','N'},
		{0x18,'o','O'},
		{0x19,'p','P'},
		{0x10,'q','Q'},
		{0x13,'r','R'},
		{0x1F,'s','S'},
		{0x14,'t','T'},
		{0x16,'u','U'},
		{0x2f,'v','V'},
		{0x11,'w','W'},
		{0x2d,'x','X'},
		{0x15,'y','Y'},
		{0x2c,'z','Z'},
		{0x2,0x31,'!'},
		{0x3,0x32,'@'},
		{0x4,0x33,'#'},
		{0x5,0x34,'$'},
		{0x6,0x35,'%'},
		{0x7,0x36,'^'},
		{0x8,0x37,'&'},
		{0x9,0x38,'*'},
		{0xa,0x39,'('},
		{0xb,0x30,')'},
		{0xc,'-','_'},
		{0xd,'=','+'},
		{0x1a,'[','{'},
		{0x1b,']','}'},
		{0x27,';',':'},
		{0x28,'\'','"'},
		{0x29,'`','~'},
		{0x2b,'\\','|'},
		{0x33,',','<'},
		{0x34,'.','>'},
		{0x35,'/','?'},
		{0x39,' ', 0xff},
		{0x1d,'^',0xff},
		{0x2A,0xff,0xff},
		{0x36,0xff,0xff},
		{0x1c,'\n','\n'},
		{0xff,0xff,0xff}
};

void kbHandler()
{
		unsigned char scancode;
		scancode = inb(0x60);
		if(!order )
		{
				int i =0;
				for(;scan_lookup_array[i].scan_code!=0xff;i++)
				{
						if(scan_lookup_array[i].scan_code == scancode)
								break;
				}
				if(scan_lookup_array[i].value == '^')
				{
						shiftcount = 0;
						order  = 1;
						ctrlcount = 1;
				}
				else if(scan_lookup_array[i].scan_code == 0x2A ||scan_lookup_array[i].scan_code == 0x36)
				{
						shiftcount = 1;
						order = 1;
				} 
				else if(scan_lookup_array[i].scan_code !=0xff)
				{
						shiftcount = 0;
						ctrlcount = 0;
						order = 1;
						if(gets_triggered){
														if(scan_lookup_array[i].value == '\n')
														{
																						buf[counter++] = '\0';
																						gets_triggered = 0;
                      temp+=(160-linePos);
                      linePos=0;
                      CHECK_FOR_FLUSH1();
														}
														else{
																						buf[counter++] = scan_lookup_array[i].value;
						                *(temp) = scan_lookup_array[i].value;
                      temp+=2;
                      linePos+=2;
                      CHECK_FOR_FLUSH1();
              }
						}

				}
		}
		else if(shiftcount)
		{
				int i =0;
				for(;scan_lookup_array[i].scan_code!=0xff;i++)
				{
						if(scan_lookup_array[i].scan_code == scancode)
								break;
				}
				if(scan_lookup_array[i].scan_code !=0xff && (scan_lookup_array[i].scan_code != 0x2A && scan_lookup_array[i].scan_code != 0x36))
				{
						shiftcount = 0;
						order = 2;
						if(gets_triggered){
														if(scan_lookup_array[i].shiftValue == '\n')
														{
																						buf[counter++] = '\0';
																						gets_triggered = 0;
                      temp+=(160-linePos);
                      linePos = 0;
                      CHECK_FOR_FLUSH1();
														}
														else{
																						buf[counter++] = scan_lookup_array[i].shiftValue;
						                *(temp) = scan_lookup_array[i].shiftValue;
                      temp+=2;
                      linePos+=2;
                      CHECK_FOR_FLUSH1();
              }
						}

				}

		}
		else if(ctrlcount)
		{
				int i =0;
				for(;scan_lookup_array[i].scan_code!=0xff;i++)
				{
						if(scan_lookup_array[i].scan_code == scancode)
								break;
				}
				if(scan_lookup_array[i].scan_code !=0xff && (scan_lookup_array[i].value != '^'))
				{
						*(temp) = scan_lookup_array[i].shiftValue;
       temp+=2;
       linePos+=2;
						ctrlcount = 0;
						order = 2;
                      CHECK_FOR_FLUSH1();
				}
		}
		else
				order --;

		//send reset signal to master
		outb(0x20, 0x20);
}

int gets(uint64_t var)
{
char * user_buff = (char*) var;
int bytesRead = 0;

gets_triggered = 1;
__asm__("sti;");
while(gets_triggered == 1);

memcpy((void*)user_buff,(void*)buf,counter);
bytesRead = counter;
counter = 0;

return bytesRead;
}

