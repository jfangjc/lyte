#include "compile.h"

#include <stdio.h>

int main(int argc, char** argv) {
    compile_files(argc - 1, argv + 1);
    printf("Compilation finished\n");
    return 0;
}
