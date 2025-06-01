#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "scanner.h"
#include "parser.h"
#include "gen.h"

#include "ht.h"

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

    else if (type == TOK_S8) { return "s8"; }
    else if (type == TOK_S16) { return "s16"; }
    else if (type == TOK_S32) { return "s32"; }
    else if (type == TOK_S64) { return "s64"; }
    else if (type == TOK_S128) { return "s128"; }
    else if (type == TOK_U8) { return "u8"; }
    else if (type == TOK_U16) { return "u16"; }
    else if (type == TOK_U32) { return "u32"; }
    else if (type == TOK_U64) { return "u64"; }
    else if (type == TOK_U128) { return "u128"; }
    else if (type == TOK_F32) { return "f32"; }
    else if (type == TOK_F64) { return "f64"; }
    else if (type == TOK_F128) { return "f128"; }

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
    //struct program_ast* program = parse_program();
    //gen("./test.s", program);
    char* test[8] = {"a", "abd", "asdas", "rewr",
    "ewrjwej", "vndfjk", "438vjvje_32j", "d"};
    int len[8] = {1, 3, 5, 4, 7, 6, 12, 1};
    struct ht* ht = ht_create();
    struct token* tokens[8];

    for (int i = 0; i < 8; i++) {
        struct token* token = malloc(sizeof(struct token));
        tokens[i] = token;
        token->start_pos = test[i];
        token->length = len[i];
        ht_insert(ht, token);
    }

    for (int i = 0; i < 8; i++) {
        printf("< %s > ", ht_lookup(ht, tokens[i])->start_pos);
    }
    printf("%i : %i", ht->capacity, ht->size);

    printf("Compilation finished\n");
    return 0;
}
