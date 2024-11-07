#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "scanner.h"
struct AST {
    struct token token;
    struct AST *left;
    struct AST *right;
};

#endif
