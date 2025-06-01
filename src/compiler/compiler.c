#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "gen.h"

int main(int argc, char** argv){
    for (int i = 1; i < argc; i++) {
        read_file(argv[i]);
        struct program_ast* program = parse_program();

        int len = strlen(argv[i]);
        argv[i][len - 2] = 's';
        argv[i][len - 1] = '\0';
        
        gen(argv[i], program);
    }

    printf("Compilation finished\n");
    return 0;
}
