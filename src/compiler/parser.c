#include "parser.h"

#include <stdlib.h>
#include "lexer.h"

struct AST *parse_statement(struct AST *statement);

int parser(struct token token) {
    struct AST *statement = malloc(sizeof(struct AST*));
    return 0;
}
