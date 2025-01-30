#include <stdio.h>

#include "scanner.h"
#include "common.h"

static char* type_word(int type) {
    if (type == TOK_SI8) { return "si8"; }
    else if (type == TOK_SI16) { return "si16"; }
    else if (type == TOK_SI32) { return "si32"; }
    else if (type == TOK_SI64) { return "si64"; }
    else if (type == TOK_UI8) { return "ui8"; }
    else if (type == TOK_UI16) { return "ui16"; }
    else if (type == TOK_UI32) { return "ui32"; }
    else if (type == TOK_UI64) { return "ui64"; }
    else if (type == TOK_NUM) { return "number"; }
    else if (type == TOK_IDENTIFIER) { return "identifier"; } 

    else if (type == TOK_FUNCTION) { return "fn"; }
    else if (type == TOK_CONST) { return "def"; }
    else if (type == TOK_VAR) { return "var"; }
    else if (type == TOK_RETURN) { return "return"; }

    char* symbol = " ";
    sprintf(symbol, "%c", type);
	return symbol;
}

int main(int argc, char** argv){
    read_file("./test.lt");
    struct token* token = next();
    while (token != NULL) {
        //printf("%s: %s \n", curr->value, (curr->type < 256 ? curr->value : type_word(curr->type)));
        printf("%s\n", type_word(token->type));
        //printf("%c: %i\n", *(token->start_pos), token->length);
        token = next();
    }
    printf("Compilation finished\n");
    return 0;
}
