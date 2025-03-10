#include <stdio.h>

#include "common.h"

#include "parser.h"
#include "scanner.h"


static char* type_word(int type) {
    if (type == TOK_SI8) { return "si8"; }
    else if (type == TOK_SI16) { return "si16"; }
    else if (type == TOK_SI32) { return "si32"; }
    else if (type == TOK_SI64) { return "si64"; }
    else if (type == TOK_UI8) { return "ui8"; }
    else if (type == TOK_UI16) { return "ui16"; }
    else if (type == TOK_UI32) { return "ui32"; }
    else if (type == TOK_UI64) { return "ui64"; }
    else if (type == TOK_NULL) { return "null"; }
    else if (type == TOK_ADDR) { return "addr"; }

    else if (type == TOK_FN) { return "fn"; }
    else if (type == TOK_CONST) { return "def"; }
    else if (type == TOK_VAR) { return "var"; }
    else if (type == TOK_RETURN) { return "return"; }

    else if (type == TOK_NUM) { return "number"; }
    else if (type == TOK_STRING) { return "string"; }
    else if (type == TOK_CHAR) { return "char"; }
    else if (type == TOK_ID) { return "identifier"; } 

    char* symbol = " ";
    sprintf(symbol, "%c", type);
	return symbol;
}

int main(int argc, char** argv){
    read_file("./test.lt");
    /*struct token* token = next_token();
    while (token != NULL) {
        printf("<");
        for (int i = 0; i < token->length; i++) {
            printf("%c", token->start_pos[i]);
        }
        printf("> ");
        token = next_token();
    }*/
    /*struct token* token = next_token();
   while (token != NULL) {
        printf("%s ", type_word(token->type));
        token = next_token();
    }*/
    parse_program();
    printf("Compilation finished\n");
    return 0;
}
