gcc -c -O1 -std=c99 -Wall -Werror -fPIC -march=amdfam10 -g3 -Wno-deprecated-declarations -D_XOPEN_SOURCE=600 -nostdinc -Iinclude -msoft-float -mno-sse -mno-red-zone -fno-builtin -fno-stack-protector -o obj/sys/process.o sys/process.c
sys/process.c: In function 'malloc_pcb':
sys/process.c:67:5: error: implicit declaration of function 'copyHelper' [-Werror=implicit-function-declaration]
     copyHelper(fun_ptr,pcb->pname,0,cstrlen(fun_ptr)); 
     ^
sys/process.c:67:37: error: implicit declaration of function 'cstrlen' [-Werror=implicit-function-declaration]
     copyHelper(fun_ptr,pcb->pname,0,cstrlen(fun_ptr)); 
                                     ^
In file included from include/sys/process.h:4:0,
                 from sys/process.c:1:
include/sys/pmap.h:26:6: error: statement with no effect [-Werror=unused-value]
      __m_kva - KERNBASE;     \
      ^
sys/process.c:80:16: note: in expansion of macro 'PADDR'
     pcb->cr3 = PADDR(pml4a);
                ^
sys/process.c:80:14: error: void value not ignored as it ought to be
     pcb->cr3 = PADDR(pml4a);
              ^
sys/process.c: In function 'schedule':
sys/process.c:194:9: error: 'tss' undeclared (first use in this function)
         tss.rsp0 =(uint64_t)(&(next->kstack[511]));    
         ^
sys/process.c:194:9: note: each undeclared identifier is reported only once for each function it appears in
sys/process.c: In function 'function1':
sys/process.c:252:9: error: implicit declaration of function 'print' [-Werror=implicit-function-declaration]
         print("\nHello inside while\n");
         ^
In file included from include/sys/memlayout.h:4:0,
                 from include/sys/pmap.h:5,
                 from include/sys/process.h:4,
                 from sys/process.c:1:
sys/process.c: In function 'region_alloc':
include/sys/defs.h:42:1: error: expected expression before 'do'
 do \
 ^
sys/process.c:265:22: note: in expansion of macro 'ROUNDDOWN'
     void *va_start = ROUNDDOWN(va, PGSIZE);
                      ^
include/sys/defs.h:49:1: error: expected expression before 'do'
 do \
 ^
sys/process.c:266:20: note: in expansion of macro 'ROUNDUP'
     void *va_end = ROUNDUP((char*)va+len, PGSIZE);
                    ^
sys/process.c:274:13: error: implicit declaration of function 'panic' [-Werror=implicit-function-declaration]
             panic("Running out of memory\n");
             ^
sys/process.c: In function 'load_icode':
sys/process.c:337:23: error: implicit declaration of function 'is_file_exists' [-Werror=implicit-function-declaration]
     uint64_t offset = is_file_exists(filename);
                       ^
sys/process.c:345:9: error: unknown type name 'Elf_hdr'
         Elf_hdr *elf = (Elf_hdr *) (&_binary_tarfs_start + offset);
         ^
sys/process.c:345:25: error: 'Elf_hdr' undeclared (first use in this function)
         Elf_hdr *elf = (Elf_hdr *) (&_binary_tarfs_start + offset);
                         ^
sys/process.c:345:34: error: expected expression before ')' token
         Elf_hdr *elf = (Elf_hdr *) (&_binary_tarfs_start + offset);
                                  ^
sys/process.c:347:9: error: unknown type name 'Elf64_Phdr'
         Elf64_Phdr *ph, *eph;
         ^
sys/process.c:349:15: error: 'Elf64_Phdr' undeclared (first use in this function)
         ph = (Elf64_Phdr *) ((uint8_t *) elf + elf->e_phoff);
               ^
sys/process.c:349:27: error: expected expression before ')' token
         ph = (Elf64_Phdr *) ((uint8_t *) elf + elf->e_phoff);
                           ^
sys/process.c:351:23: error: request for member 'e_phnum' in something not a structure or union
         eph = ph + elf->e_phnum;
                       ^
sys/process.c:354:19: error: request for member 'p_type' in something not a structure or union
             if (ph->p_type == ELF_PROG_LOAD) {
                   ^
sys/process.c:354:31: error: 'ELF_PROG_LOAD' undeclared (first use in this function)
             if (ph->p_type == ELF_PROG_LOAD) {
                               ^
sys/process.c:359:23: error: request for member 'p_filesz' in something not a structure or union
                 if (ph->p_filesz > ph->p_memsz)
                       ^
sys/process.c:359:38: error: request for member 'p_memsz' in something not a structure or union
                 if (ph->p_filesz > ph->p_memsz)
                                      ^
sys/process.c:372:45: error: request for member 'p_vaddr' in something not a structure or union
                 region_alloc(pcb, (void*) ph->p_vaddr, ph->p_memsz);
                                             ^
sys/process.c:372:58: error: request for member 'p_memsz' in something not a structure or union
                 region_alloc(pcb, (void*) ph->p_vaddr, ph->p_memsz);
                                                          ^
sys/process.c:373:46: error: request for member 'p_offset' in something not a structure or union
                 copyHelper( (void *) elf + ph->p_offset,(char*) ph->p_vaddr,0
                                              ^
sys/process.c:373:67: error: request for member 'p_vaddr' in something not a structure or union
                 copyHelper( (void *) elf + ph->p_offset,(char*) ph->p_vaddr,0
                                                                   ^
sys/process.c:374:25: error: expected ')' before 'ph'
                         ph->p_filesz);
                         ^
sys/process.c:376:23: error: request for member 'p_filesz' in something not a structure or union
                 if (ph->p_filesz < ph->p_memsz)
                       ^
sys/process.c:376:38: error: request for member 'p_memsz' in something not a structure or union
                 if (ph->p_filesz < ph->p_memsz)
                                      ^
sys/process.c:377:38: error: request for member 'p_vaddr' in something not a structure or union
                     memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz
                                      ^
sys/process.c:377:52: error: request for member 'p_filesz' in something not a structure or union
                     memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz
                                                    ^
sys/process.c:377:69: error: request for member 'p_memsz' in something not a structure or union
                     memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz
                                                                     ^
sys/process.c:378:33: error: request for member 'p_filesz' in something not a structure or union
                             - ph->p_filesz);
                                 ^
sys/process.c:386:34: error: request for member 'p_vaddr' in something not a structure or union
                 vm->vm_start = ph->p_vaddr;
                                  ^
sys/process.c:387:47: error: request for member 'p_memsz' in something not a structure or union
                 vm->vm_end = vm->vm_start + ph->p_memsz;
                                               ^
sys/process.c:388:33: error: request for member 'p_memsz' in something not a structure or union
                 vm->vm_mmsz = ph->p_memsz;
                                 ^
sys/process.c:391:34: error: request for member 'p_flags' in something not a structure or union
                 vm->vm_flags = ph->p_flags;
                                  ^
sys/process.c:392:35: error: request for member 'p_offset' in something not a structure or union
                 vm->vm_offset = ph->p_offset;  
                                   ^
sys/process.c:401:25: error: request for member 'e_entry' in something not a structure or union
         pcb->entry = elf->e_entry;
                         ^
In file included from include/sys/process.h:4:0,
                 from sys/process.c:1:
sys/process.c: In function 'fork_process':
include/sys/pmap.h:26:6: error: statement with no effect [-Werror=unused-value]
      __m_kva - KERNBASE;     \
      ^
sys/process.c:504:22: note: in expansion of macro 'PADDR'
     child_pcb->cr3 = PADDR(pml4e);
                      ^
sys/process.c:504:20: error: void value not ignored as it ought to be
     child_pcb->cr3 = PADDR(pml4e);
                    ^
In file included from include/sys/process.h:4:0,
                 from sys/process.c:1:
include/sys/pmap.h:26:6: error: statement with no effect [-Werror=unused-value]
      __m_kva - KERNBASE;     \
      ^
sys/process.c:533:97: note: in expansion of macro 'PADDR'
     boot_map_segment( (pml4e_t *)child_pcb->pml4e,(uintptr_t)child_pcb->stack, 4096, (uintptr_t)PADDR(tmp_stack),PTE_W|PTE_P|PTE_U); 
                                                                                                 ^
sys/process.c:533:5: error: invalid use of void expression
     boot_map_segment( (pml4e_t *)child_pcb->pml4e,(uintptr_t)child_pcb->stack, 4096, (uintptr_t)PADDR(tmp_stack),PTE_W|PTE_P|PTE_U); 
     ^
sys/process.c: In function 'execvpe':
sys/process.c:773:5: error: implicit declaration of function 'cstrcpy' [-Werror=implicit-function-declaration]
     cstrcpy(pathname,tmp_pathname);
     ^
cc1: all warnings being treated as errors
Makefile:86: recipe for target 'obj/sys/process.o' failed
make: *** [obj/sys/process.o] Error 1
