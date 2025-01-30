#include "parser.h"

#include <stdlib.h>

#include "scanner.h"
#include "common.h"

void create_ast() {
}

struct definition* parser(struct token* tokens) {
    if (tokens == NULL) {
        return NULL;
    }
    if (tokens -> type == TOK_FUNCTION) {
        return create_definition(tokens);
    }
    return parser(tokens -> next);
}
