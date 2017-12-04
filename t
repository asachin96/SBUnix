gcc -c -O1 -std=c99 -Wall -Werror -fPIC -march=amdfam10 -g3 -Wno-deprecated-declarations -D_XOPEN_SOURCE=600 -nostdinc -Iinclude -msoft-float -mno-sse -mno-red-zone -fno-builtin -fno-stack-protector -o obj/sys/idt.o sys/idt.c
sys/idt.c:131:20: error: unknown type name 'regsiters_t'
 void commonHandler(regsiters_t regs){
                    ^
sys/idt.c:46:13: error: 'divide_by_zero_handler' defined but not used [-Werror=unused-function]
 static void divide_by_zero_handler(registers_t regs)
             ^
sys/idt.c:53:13: error: 'tss_fault_handler' defined but not used [-Werror=unused-function]
 static void tss_fault_handler(registers_t regs)
             ^
sys/idt.c:59:13: error: 'gpf_handler' defined but not used [-Werror=unused-function]
 static void gpf_handler(registers_t regs)
             ^
sys/idt.c:65:13: error: 'page_fault_handler' defined but not used [-Werror=unused-function]
 static void page_fault_handler(registers_t regs)
             ^
cc1: all warnings being treated as errors
Makefile:86: recipe for target 'obj/sys/idt.o' failed
make: *** [obj/sys/idt.o] Error 1
