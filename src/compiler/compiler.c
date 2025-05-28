#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "scanner.h"
#include "parser.h"
#include "gen.h"

static char* type_word(int type) {
    if (type == TOK_EOF) { return "eof"; }
    else if (type == TOK_LET) { return "let"; }
    else if (type == TOK_FN) { return "fn"; }
    else if (type == TOK_VAR) { return "var"; }
    else if (type == TOK_ADDR) { return "addr"; }

    else if (type == TOK_IF) { return "if"; }
    else if (type == TOK_ELSE) { return "else"; }

    else if (type == TOK_FOR) { return "for"; }
    else if (type == TOK_BREAK) { return "break"; }
    else if (type == TOK_CONTINUE) { return "continue"; }
    else if (type == TOK_RETURN) { return "return"; }

    else if (type == TOK_ID) { return "identifier"; }

    else if (type == TOK_SI8) { return "si8"; }
    else if (type == TOK_SI16) { return "si16"; }
    else if (type == TOK_SI32) { return "si32"; }
    else if (type == TOK_SI64) { return "si64"; }
    else if (type == TOK_UI8) { return "ui8"; }
    else if (type == TOK_UI16) { return "ui16"; }
    else if (type == TOK_UI32) { return "ui32"; }
    else if (type == TOK_UI64) { return "ui64"; }

    else if (type == TOK_ASSIGN) { return "assign"; }

    else if (type == TOK_EQEQ) { return "eq"; }
    else if (type == TOK_NOTEQ) { return "noteq"; }
    else if (type == TOK_GTEQ) { return "gteq"; }
    else if (type == TOK_LTEQ) { return "lteq"; }

    else if (type == TOK_OR) { return "or"; }
    else if (type == TOK_AND) { return "and"; }
    else if (type == TOK_NOT) { return "not"; }

    else if (type == TOK_INT) { return "int"; }
    else if (type == TOK_FLOAT) { return "float"; }
    else if (type == TOK_CHAR) { return "char"; }
    else if (type == TOK_STRING) { return "string"; }

    else if (type == TOK_IMPORT) { return "import"; }
    else if (type == TOK_EXPORT) { return "export"; }

    char* symbol = malloc(16);
    sprintf(symbol, "%c", (char)type);
	return symbol;
}

int main(int argc, char** argv){
    for (int i = 0; i < argc; i++) {
        printf("input: %s\n", argv[i]);
        type_word(0);
    }

    read_file("./test.lt");
    /*struct token* token = next_token();
    while (token != NULL) {
        printf("<");
        for (int i = 0; i < token->length; i++) {
            printf("%c", token->start_pos[i]);
        }
        printf(" : %s", type_word(token->type));
        printf("> ");
        token = next_token();
    }*/
    struct program_ast* program = parse_program();
    gen("./test.s", program);
    
    printf("Compilation finished\n");
    return 0;
}
