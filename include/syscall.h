#ifndef _SYSCALL_H
#define _SYSCALL_H

#define __NR_read 0
#define __NR_write 1
#define __NR_brk 2
#define __NR_fork 3
#define __NR_execvpe 4
#define __NR_waitpid 5
#define __NR_exit 6
#define __NR_getpid 7
#define __NR_getppid 8
#define __NR_listprocess 9
#define __NR_opendir 10
#define __NR_readdir 11
#define __NR_closedir 12
#define __NR_open 13
#define __NR_close 14
#define __NR_sleep 15
#define __NR_clear 16
#define __NR_seek 17
#define __NR_kill 18
#define __NR_getcwd 19
#define __NR_chdir 20
#define __NR_signal 21
#define __NR_pipe 22
#define NUM_SYSCALLS 23

#endif
