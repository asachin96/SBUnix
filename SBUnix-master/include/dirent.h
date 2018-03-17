#ifndef _DIRENT_H
#define _DIRENT_H
#include <sys/defs.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define DIRECTORY 1
#define FILE 2

#define PIPE_SIZE 100
#define PIPE_READ 3
#define PIPE_WRITE 4

typedef struct file {
    uint64_t first;
    uint64_t last;
    uint64_t current;
    char name[20];
    struct file* child[20];
    int type;
}file;

typedef struct dirent {
    uint64_t ino;
    char name[20];
}dirent;

typedef struct dir {
    file* filenode;
    uint64_t curr;
    dirent curr_dirent;
}DIR;

typedef struct FD {
    uint64_t current, permission;
    file* node;
}FD;

file* root;

DIR* opendir(char*);
struct dirent* readdir(DIR*);
int closedir(DIR*);
void* fileLookup(char*);

#endif
