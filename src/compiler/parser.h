#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "lexer.h"
struct AST {
    struct token token;
    struct AST *next;
};

#endif
