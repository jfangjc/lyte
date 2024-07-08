#include <stdio.h>

#include "lexer.h"

int main(int argc, char** argv){
    char* content = read_file("../../Test.lt");
    int i = 0;

    while (content[i] != '\0') {
        struct token result = next(content, &i);
        printf("%s, line: %i, char: %i, type: %i \n", result.value, result.line_num, result.char_num, result.type);
    }

    //fprintf(stderr, "Error\n");
    printf("Compilation finished\n");
    return 0;
}
