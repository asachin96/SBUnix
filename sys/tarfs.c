#include <sys/tarfs.h>
#include <sys/kstring.h>
#include <sys/defs.h>

void* lookup(char *file_name) {
    HEADER *header = (HEADER*) &_binary_tarfs_start;
    int size_of_file = 0; 
    char *p;
    
    do {
        size_of_file = oct_to_dec(atoi(header->size));
        p = (char*)(header + 1);
        
        if (kstrcmp(file_name, header->name) == 0) {   
            return (void*)p;
        }
       
        if (size_of_file > 0) {
            header = header + size_of_file / (sizeof(HEADER) + 1) + 2;
        } else {
            header = header + 1;
        }
    } while(header < (HEADER*)&_binary_tarfs_end);
    
    return NULL;
}
