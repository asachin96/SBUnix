#include <stdlib.h>

void _start(uint64_t start) {
    exit(main(*(int*)(&start+2), (char**)(&start+3)));
}

