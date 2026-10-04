#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "lexer.h"

struct type {
    int kind; // 0: named, 1: pointer, 2: reference
    union {
        struct token* name;
        struct type* base;
    };
};

struct program_ast {
    struct token* module_name;
    char* module_path;
    struct import_decl* imports;
    struct export_decl* exports;
    struct type_decl* type_decls;
    struct var_decl* var_decls;
    struct fn_decl* fn_decls;
};

struct fn_decl {
    struct token* name;
    struct param* params;
    struct stmt* body;
    struct type* type;
    int is_exported;
    int is_unsafe;
    struct fn_decl* next;
};

struct var_decl {
    struct token* name;
    struct type* type;
    struct expr* value;
    int is_const;
    int is_exported;
    struct var_decl* next;
};

struct import_decl {
    struct token* module_name;
    char* module_path;
    struct token* alias;
    struct import_decl* next;
};

struct export_decl {
    struct token* name;
    struct export_decl* next;
};

struct type_decl {
    struct token* name;
    struct type* alias;
    int is_exported;
    struct type_decl* next;
};

struct param {
    struct token* name;
    struct type* type;
    struct param* next;
};

union stmts {
    struct var_decl* var_decl;
    struct if_stmt* if_stmt;
    struct for_stmt* for_stmt;
    struct unsafe_stmt* unsafe_stmt;
    struct return_stmt* return_stmt;
    struct expr_stmt* expr_stmt;
};

enum StmtType { STMT_VAR, STMT_IF, STMT_FOR, STMT_UNSAFE, STMT_BREAK, STMT_CONTINUE, STMT_RETURN, STMT_EXPR };

struct stmt {
    enum StmtType type;
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
    struct stmt* init;
    struct expr* cond;
    struct expr* step;
    struct stmt* body;
};

struct unsafe_stmt {
    struct stmt* body;
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

enum ExprType { EXPR_ASSIGN, EXPR_BOOLEAN, EXPR_EQUALITY, EXPR_ARITH, EXPR_UNARY, EXPR_CALL, EXPR_ID, EXPR_VALUE };

struct expr {
    enum ExprType type;
    union exprs exprs;
};

struct assign_expr {
    struct token* id;
    struct expr* value;
    int op;
};

struct boolean_expr {
    struct expr* left;
    struct expr* right;
    int op;
};

struct equality_expr {
    struct expr* left;
    struct expr* right;
    int op;
};

struct arith_expr {
    struct expr* left;
    struct expr* right;
    int op;
};

struct unary_expr {
    int op;
    struct expr* expr;
};

struct call_expr {
    struct token* id;
    char* name;
    struct arg* args;
};

struct arg {
    struct expr* value;
    struct arg* next;
};

struct program_ast* parse_program(void);

#endif
