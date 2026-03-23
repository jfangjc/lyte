#include <stdio.h>
#include <string.h>

#include "codegen.h"
#include "parser.h"
#include "lexer.h"

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        read_file(argv[i]);
        struct program_ast* program = parse_program();

        size_t len = strlen(argv[i]);
        argv[i][len - 2] = 's';
        argv[i][len - 1] = '\0';

        gen(argv[i], program);
    }

    printf("Compilation finished\n");
    return 0;
}
