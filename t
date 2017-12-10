gcc -c -Og -std=c99 -Wall -Werror -fPIC -march=amdfam10 -g3 -Wno-deprecated-declarations -D_XOPEN_SOURCE=600 -nostdinc -Iinclude -msoft-float -mno-sse -mno-red-zone -fno-builtin -fno-stack-protector -o obj/bin/sh/sh.o bin/sh/sh.c
In file included from bin/sh/sh.c:2:0:
include/unistd.h:6:5: error: conflicting types for ‘open’
 int open(const char *pathname, int flags);
     ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:21:5: note: previous declaration of ‘open’ was here
 int open(char *path, int flags);
     ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:7:5: error: conflicting types for ‘close’
 int close(int fd);
     ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:25:6: note: previous declaration of ‘close’ was here
 void close(int file_d);
      ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:8:9: error: conflicting types for ‘read’
 ssize_t read(int fd, void *buf, size_t count);
         ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:22:10: note: previous declaration of ‘read’ was here
 uint64_t read(uint64_t fd, void *buf, uint64_t nbytes);
          ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:9:9: error: conflicting types for ‘write’
 ssize_t write(int fd, const void *buf, size_t count);
         ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:23:5: note: previous declaration of ‘write’ was here
 int write(int n, char *str, int len);
     ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:17:7: error: conflicting types for ‘wait’
 pid_t wait(int *status);
       ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:34:7: note: previous declaration of ‘wait’ was here
 pid_t wait(uint64_t *status);
       ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:18:5: error: conflicting types for ‘waitpid’
 int waitpid(int pid, int *status);
     ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:35:7: note: previous declaration of ‘waitpid’ was here
 pid_t waitpid(pid_t pid, uint64_t *status, int options);
       ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:20:14: error: conflicting types for ‘sleep’
 unsigned int sleep(unsigned int seconds);
              ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:9:5: note: previous declaration of ‘sleep’ was here
 int sleep(int seconds);
     ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:27:5: error: conflicting types for ‘mkdir’
 int mkdir(const char *pathname, int mode);
     ^
In file included from bin/sh/sh.c:1:0:
include/stdlib.h:26:5: note: previous declaration of ‘mkdir’ was here
 int mkdir(char *path);
     ^
bin/sh/sh.c: In function ‘fork_and_execvpe’:
bin/sh/sh.c:100:13: error: too many arguments to function ‘waitpid’
             waitpid(pid, NULL, 0);
             ^
In file included from bin/sh/sh.c:2:0:
include/unistd.h:18:5: note: declared here
 int waitpid(int pid, int *status);
     ^
bin/sh/sh.c: In function ‘main’:
bin/sh/sh.c:298:19: error: ‘curdir’ undeclared (first use in this function)
             chdir(curdir);
                   ^
bin/sh/sh.c:298:19: note: each undeclared identifier is reported only once for each function it appears in
Makefile:87: recipe for target 'obj/bin/sh/sh.o' failed
make[1]: *** [obj/bin/sh/sh.o] Error 1
Makefile:77: recipe for target 'rootfs/bin/sh' failed
make: *** [rootfs/bin/sh] Error 2
