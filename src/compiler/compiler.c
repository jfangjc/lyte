#include <stdio.h>

#include "scanner.h"
#include "common.h"

static char* type_word(int type) {
    if (type == TOK_SI8) { return "si8"; }
    else if (type == TOK_SI16) { return "si16"; }
    else if (type == TOK_SI32) { return "si32"; }
    else if (type == TOK_SI64) { return "si64"; }
    else if (type == TOK_SI128) { return "si128"; }
    else if (type == TOK_UI8) { return "ui8"; }
    else if (type == TOK_UI16) { return "ui16"; }
    else if (type == TOK_UI32) { return "ui32"; }
    else if (type == TOK_UI64) { return "ui64"; }
    else if (type == TOK_UI128) { return "ui128"; }
    // implement more complex float type later
    else if (type == TOK_FLOAT) { return "float"; }
    else if (type == TOK_DFLOAT) { return "dfloat"; }
    else if (type == TOK_TYPE) { return "type"; }
    else if (type == TOK_FUNCTION) { return "function"; }
	return "identifier";
}

int main(int argc, char** argv){
    char *src = read_file("./test.lt");
    struct token *head = next(src);
    struct token *curr = head;
    while (curr->value) {
        printf("%s: %s \n", curr->value, (curr->type < 256 ? curr->value : type_word(curr->type)));
        curr = curr -> next;
    }
    printf("Compilation finished\n");
    return 0;
}
