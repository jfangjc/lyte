#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "scanner.h"

struct program_ast {
    struct fn_decl* fn_decls;
};

struct fn_decl {
    struct token* fn_name;
    struct param* fn_params;
    struct stmt* fn_body;
    int fn_type;
    struct fn_decl* next;
};

struct param {
    int type;
    int value;
    struct token* token;
};

struct arg {
    struct token *name;
    struct arg* next;
};

struct var_decl {
    struct token* var_name;
    int var_type;
    struct expr* var_value;
};

struct expr {
    struct token* nodes;
    struct toekn *operation;
    struct expr* next;
};

union stmts {
    struct var_decl* var_decl;
    struct if_stmt* if_stmt;
    struct stmt* compound_stmt;
    struct stmt* for_stmt;
    struct stmt* while_stmt;
    struct stmt* break_stmt;
    struct stmt* continue_stmt;
    struct stmt* return_stmt;
    struct stmt* expr_stmt;
};

struct stmt {
    union stmts stmts;
    struct stmt* next;
};

struct if_stmt {
    struct expr *expr;
    struct stmt* stmt;
    struct elseif_stmt* elseif_stmt;
    struct else_stmt* else_stmt;
};

struct elseif_stmt {
    struct expr *expr;
    struct stmt* stmt;
};

struct else_stmt {
    struct stmt* stmt;
};

struct for_stmt {
    struct var_decl* var;
    struct stmt* stmt;
}

struct program_ast* parse_program();

#endif
