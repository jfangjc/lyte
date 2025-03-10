#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "scanner.h"

struct program_ast {
    struct fn_decl* fn_decls;
    struct fn_look_up* fn_look_up;
};

struct fn_look_up {
    struct fn_decl* fn;
    char* name;
};

struct fn_decl {
    struct token* fn_name;
    struct node* fn_params;
    struct node* fn_body;
    int return_type;
    struct fn_decl* next;
};

struct fn_call {
    struct token* fn_name;
    struct node* fn_params;
};

struct var_decl {
    struct token* var_name;
    int var_type;
    struct node* var_value;
};

struct expr {
    struct node* nodes;
    struct toekn* operation;
};

struct node {
    union {
        struct var_decl* var_decl;
        struct fn_call* fn_call;
    };
    struct node* next;
};

struct program_ast* parse_program();

#endif
