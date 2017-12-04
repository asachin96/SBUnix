//
// irq_handler.c -- High level interrupt service routines and interrupt request handlers
//                  for IRQ0 to IRQ15 
//

#include <sys/defs.h>
#include <syscall.h>
#include <sys/irq_common.h>
#include <sys/types.h>
#include <sys/util.h>

extern void kbHandler();
#if 0
int gets(uint64_t addr)
{
    char *user_buf = (char*) addr;
    int count;

    flag = 1;
    sti;

    last_addr = get_video_addr();
    while (flag == 1);
    memcpy((void *)user_buf, (void *)buf, counter);
    count = counter;
    counter = 0;
    return count;
}

void init_keyboard()
{
    lastKeyPressed = NOKEY;
    last_addr = NULL;
    flag = counter = 0;
}

/* Handles the keyboard interrupt */
static void keyboard_handler(registers_t regs)
{
    uint8_t scancode, val;

    // Read from the keyboard's data buffer
    scancode = inb(0x60);
    if (scancode & 0x80) {
        // KeyPressUp Scancodes
        if (scancode == 170 || scancode == 184 || scancode == 157) {
            lastKeyPressed = NOKEY;
        }
    } else {

        /* Scancodes for CTRL , SHIFT , ALT press down are 29, 42, 56 
         * SHIFT : Prints characters in caps and secondary characters on keys
         * ALT   : Prints ~ tilt character followed by the next character pressed
         * CTRL  : Prints ^ character followed by the next character pressed
         */
        if (scancode == 42)
            lastKeyPressed = SHIFT;
        else if (scancode == 56)
            lastKeyPressed = ALT;
        else if (scancode == 29)
            lastKeyPressed = CTRL;
        else {
            switch (lastKeyPressed) {
                case NOKEY:
                    val = kbsmall[scancode];
                    break;
                case CTRL:
                    val = kbsmall[scancode];
                    break;
                case ALT:
                    val = kbsmall[scancode];
                    break;
                case SHIFT:
                    val = kbcaps[scancode];
                    break;
                default:
                    val = 0;
                    break;
            }
            if (flag == 1) {
                if (val == '\n') {
                    buf[counter++] = '\0';
                    putchar(val);
                    flag = 0;
                } else if (val == '\b') {
                    if (get_video_addr() > last_addr) {
                        putchar(val);
                        counter--;
                    }
                } else {
                    buf[counter++] = val;
                    putchar(val);
                }
            }
        }
    }       
}
#endif
/****************************************************************
 IRQ Common Handler Routine
****************************************************************/
void irq_handler(registers_t regs)
{
    switch (regs.int_no) {
        case 32:
            // Called directly from irq0()
            break;
        case 33:
            kbHandler();
            break;
        default:
            break;
    }
    // Send EOI signal to Master PIC
    outb(0x20, 0x20);
}

