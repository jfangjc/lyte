#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "scanner.h"

struct program_ast {
    struct let_decl* let_decls;
    struct fn_decl* fn_decls;
};

struct let_decl {
    struct token* id;
    struct expr* value;
};

struct fn_decl {
    struct token* name;
    struct param* params;
    struct stmt* body;
    int type;
    struct fn_decl* next;
};

struct var_decl {
    struct token* name;
    int type;
    struct expr* value;
};

struct addr_decl {
    struct token* name;
    int type;
    struct expr* value;
};

struct param {
    struct token* name;
    int type;
    struct param* next;
};

union stmts {
    struct var_decl* var_decl;
    struct addr_decl* addr_decl;
    struct if_stmt* if_stmt;
    struct for_stmt* for_stmt;
    struct break_stmt* break_stmt;
    struct continue_stmt* continue_stmt;
    struct return_stmt* return_stmt;
    struct expr_stmt* expr_stmt;
};

struct stmt {
    union stmts stmt;
    struct stmt* next;
};

struct if_stmt {
    struct expr* if_cond;
    struct stmt* if_body;
    struct elseif_stmt* elseif_stmt;
    struct stmt* else_body;
};

struct elseif_stmt {
    struct expr* cond;
    struct stmt* body;
    struct elseif_stmt* next;
};

struct for_stmt {
    struct expr* cond;
    struct stmt* body;
};

struct break_stmt {
    struct stmt* stmt;
};

struct continue_stmt {
    struct stmt* stmt;
};

struct return_stmt {
    struct expr* expr;
};

union exprs {
    struct assign_expr* assign_expr;
    struct boolean_expr* boolean_expr;
    struct equality_expr* equality_expr;
    struct arith_expr* arith_expr;
    struct unary_expr* unary_expr;
    struct call_expr* call_expr;
    struct token* id;
    struct token* value_expr;
};

struct expr_stmt {
    struct expr* expr;
};

struct expr {
    union exprs exprs;
};

struct assign_expr {
    struct token* id;
    struct expr* value;
};

struct boolean_expr {
    struct token* left;
    struct expr* right;
    int op;
};

struct equality_expr {
    struct token* left;
    struct expr* right;
    int op;
};

struct arith_expr {
    struct token* left;
    struct expr* right;
    int op;
};

struct unary_expr {
    int op;
    struct expr* expr;
};

struct call_expr {
    struct token* id;
    struct arg* args;
};

struct arg {
    struct expr* value;
    struct arg* next;
};

struct program_ast* parse_program();

#endif
