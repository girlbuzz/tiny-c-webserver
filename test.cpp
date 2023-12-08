#include <iostream>
#include "syscall-macros.h"
#include <unistd.h>

int main() {
    int* initial_brk;
    asm_brk(initial_brk, 0);
    printf("%p\n", initial_brk);
    
    int* new_brk;
    asm_brk(new_brk, initial_brk+4096);
    printf("%p\n", new_brk);

    return 0;
}
