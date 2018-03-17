#ifndef _ELF_H
#define _ELF_H

#include <sys/defs.h>
#include <sys/procMgr.h> 

typedef struct elf64Hdr{
    unsigned char e_ident[16]; /* ELF identification */
    uint16_t e_type; /* Object file type */
    uint16_t e_machine; /* Machine type */
    uint32_t e_version; /* Object file version */
    uint64_t e_entry; /* Entry point address */
    uint64_t e_phoff; /* Program header offset */
    uint64_t e_shoff; /* Section header offset */
    uint32_t e_flags; /* Processor-specific flags */
    uint16_t e_ehsize; /* ELF header size */
    uint16_t e_phentsize; /* Size of program header entry */
    uint16_t e_phnum; /* Number of program header entries */
    uint16_t e_shentsize; /* Size of section header entry */
    uint16_t e_shnum; /* Number of section header entries */
    uint16_t e_shstrndx; /* Section name string table index */
} ElfHdr;

typedef struct elf64PHdr{
    uint32_t p_type; /* Type of segment */
    uint32_t p_flags; /* Segment attributes */
    uint64_t p_offset; /* Offset in file */
    uint64_t p_vaddr; /* Virtual address in memory */
    uint64_t p_paddr; /* Reserved */
    uint64_t p_filesz; /* Size of segment in file */
    uint64_t p_memsz; /* Size of segment in memory */
    uint64_t p_align; /* Alignment of segment */
} ElfPHdr;

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

typedef struct posix_header_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
} tarfsHeader;

task* createNewProc(char*, char**);
#endif
