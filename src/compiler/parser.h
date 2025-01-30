#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "scanner.h"

struct define_expr {
    int type;
    char* name;
    struct definition* arguments;
    struct definition* content;
};

struct call_expr {
    int i;
};

struct binary_expr {
    int i;
};

struct AST {
    struct token* token;
    struct AST* left;
    struct AST* right;
};

struct definition {
    int i;
};

#endif
