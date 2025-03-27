#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "common.h"
#include "error.h"

struct token* curr_token;

struct fn_decl* parse_fn_decl();
struct var_decl* parse_var_decl();
struct expr* parse_init_decl();
int parse_type();
struct token* parse_id();
struct stmt* parse_compound_stmt();
struct stmt* parse_stmt();
struct stmt* parse_if_stmt();
struct stmt* parse_for_stmt();
struct stmt* parse_while_stmt();
struct stmt* parse_break_stmt();
struct stmt* parse_continue_stmt();
struct stmt* parse_return_stmt();
struct stmt* parse_expr_stmt();
struct expr* parse_expr();
struct expr* parse_assign_expr();
struct expr* parse_cond_or_expr();
struct expr* parse_cond_and_expr();
struct expr* parse_equality_expr();
struct expr* parse_rel_expr();
struct expr* parse_additive_expr();
struct expr* parse_multiplicative_expr();
struct expr* parse_unary_expr() ;
struct expr* parse_primary_expr();
struct param* parse_param_list();
struct arg* parse_arg_list();
struct token* parse_num();

void next() {
    curr_token = next_token();
}

void expect(int type) {
    if (curr_token->type == type) {
        next();
        return;
    }
    printf("\n ERROR: %i : %c \n", type, curr_token->start_pos[0]);
    error("wrong type");
    return;
}

int match(int type) {
    return curr_token->type == type;
}

struct program_ast* parse_program() {
    struct program_ast* program = malloc(sizeof(struct program_ast));
    struct fn_decl** fn = &(program->fn_decls);
    next();

    while (curr_token != NULL) {
        *fn = parse_fn_decl();
        printf("<%i> ", (*fn)->fn_type);
        next();
        fn = &((*fn)->next);
    }
    *fn = NULL;

    return program;
}

struct fn_decl* parse_fn_decl() {
    if (curr_token->type != TOK_FN) {
        return NULL;
    }
    struct fn_decl* fn = malloc(sizeof(struct fn_decl));

    printf("fn ");
    expect(TOK_FN);

    fn->fn_name = parse_id();
    fn->fn_params = parse_param_list();

    expect(':');

    fn->fn_type = parse_type();
    fn->fn_body = parse_compound_stmt();

    return fn;
}

struct var_decl* parse_var_decl() {
    struct var_decl* var = malloc(sizeof(struct var_decl));
    printf("var ");

    if (!match(TOK_VAR)) {
        return NULL;
    }

    expect(TOK_VAR);

    var->var_name = parse_id();

    if (match('[')) {
        next();
        if (match(TOK_NUM)) {
            parse_num();
        }
        expect(']');
    }

    expect(':');

    var->var_type = parse_type();
    var->var_value = parse_init_decl();

    expect(';');
    return var;
}

struct expr* parse_init_decl() {
    printf("init_decl ");

    if (match('=')) {
        struct expr* expr = malloc(sizeof(struct expr));
        printf("assign ");
        next();
        expr = parse_expr();
         return expr;
    }
    return NULL;
}

int parse_type() {
    printf("type ");
    if (!match(TOK_SI8) && !match(TOK_SI16)
        && !match(TOK_SI32) && !match(TOK_SI64)
        && !match(TOK_UI8) && !match(TOK_UI16)
        && !match(TOK_UI32) && !match(TOK_UI64)) {
        error("Unrecognised type");
    }
    int type = curr_token->type;
    next();
    return type;

    }

struct token* parse_id() {
    printf("id ");
    expect(TOK_ID);
    return curr_token;
}

struct stmt* parse_compound_stmt() {
    printf("compound_stmt ");
    expect('{');

    struct stmt* stmt = malloc(sizeof(struct stmt));
    struct stmt* head = stmt;
    struct stmt* prev = stmt;

    if (match('}')) {
        return NULL;
    }

    while (!match('}')) {
        if (match(TOK_VAR)) {
            stmt->stmts.var_decl = parse_var_decl();
        }
        else {
            stmt->next = parse_stmt();
        }
        prev = stmt;
        stmt->next = malloc(sizeof(struct stmt));
        stmt = stmt->next;
    }

    free(stmt);
    prev->next = NULL;

    expect('}');
    return head;
}

struct stmt* parse_stmt() {
    printf("stmt ");

    switch (curr_token->type) {
        case '{':
            return parse_compound_stmt();
        case TOK_IF:
            return parse_if_stmt();
        case TOK_FOR:
            return parse_for_stmt();
        case TOK_WHILE:
            return parse_while_stmt();
        case TOK_BREAK:
            return parse_break_stmt();
        case TOK_CONTINUE:
            return parse_continue_stmt();
        case TOK_RETURN:
            return parse_return_stmt();
        default:
            return parse_expr_stmt();
    }
}


struct stmt* parse_if_stmt() {
    struct stmt* stmt = malloc(sizeof(struct stmt));
    struct if_stmt* if_stmt = malloc(sizeof(struct if_stmt));

    printf("if ");
    expect(TOK_IF);

    expect('(');

    if_stmt->expr = parse_expr();

    expect(')');

    if_stmt->stmt = parse_compound_stmt();

    int else_count = 0;
    while (match(TOK_ELSE)) {
        next();
        if (match(TOK_IF)) {
            next();
            expect('(');
            if_stmt->elseif_stmt->expr = parse_expr();
            expect(')');
            if_stmt->elseif_stmt->stmt = parse_compound_stmt();
        }
        else {
            if (else_count > 1) {
              error("Only allow one else statement");
            }
            else_count += 1;
        }
    }
    
    return stmt;
}

struct stmt* parse_for_stmt() {
    printf("for ");
    expect(TOK_FOR);
    expect('(');

    parse_expr();
    expect(';');

    parse_expr();
    expect(';');

    parse_expr();
    expect(')');
    
    parse_compound_stmt();
    return NULL;
}

struct stmt* parse_while_stmt() {
    printf("while ");
    expect(TOK_WHILE);
    expect('(');
    parse_expr();
    expect(')');
    parse_compound_stmt();
    return NULL;
}

struct stmt* parse_break_stmt() {
    printf("break ");
    expect(TOK_BREAK);
    expect(';');
    return NULL;
}

struct stmt* parse_continue_stmt() {
    printf("continue ");
    expect(TOK_CONTINUE);
    expect(';');
    return NULL;
}

struct stmt* parse_return_stmt() {
    printf("return ");
    expect(TOK_RETURN);
    if (!match(';')) {
        parse_expr();
    }
    expect(';');
    return NULL;
}

struct stmt* parse_expr_stmt() {
    struct expr* expr;
    printf("expr_stmt ");
    if (curr_token->type != ';') {
        expr = parse_expr();
    }
    expect(';');
    return NULL;
}

struct expr* parse_expr() {
    struct expr* expr;
    printf("expr ");
    expr = parse_assign_expr();
    return expr;
}

struct expr* parse_assign_expr() {
    struct expr* expr;
    struct expr* head;
    printf("assign ");
    head = parse_cond_or_expr();
    expr = head;
    while (curr_token->type == '=') {
        next();
        expr->next = parse_cond_or_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return head;
}

struct expr* parse_cond_or_expr() {
    struct expr* head;
    struct expr* expr;
    printf("or ");
    head = parse_cond_and_expr();
    expr = head;
    while (curr_token->type == TOK_OROR) {
        next();
        expr->next = parse_cond_and_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return head;
}

struct expr* parse_cond_and_expr() {
    struct expr* head;
    struct expr* expr;
    printf("and ");
    head = parse_equality_expr();
    expr = head;
    while (curr_token->type == TOK_ANDAND) {
        next();
        expr->next = parse_equality_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return head;
}

struct expr* parse_equality_expr() {
    struct expr* head;
    struct expr* expr;
    printf("equal ");
    head = parse_rel_expr();
    expr = head;
    while (curr_token->type == TOK_EQEQ
           || curr_token->type == TOK_NOTEQ) {
        next();
        expr->next = parse_rel_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return expr;    
}

struct expr* parse_rel_expr() {
    struct expr* head;
    struct expr* expr; 
    printf("rel ");
    head = parse_additive_expr();
    expr = head;
    while (curr_token->type == '<'
    || curr_token->type == TOK_LTEQ
    || curr_token->type == '>'
    || curr_token->type == TOK_GTEQ) {
        next();
        expr->next = parse_additive_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return expr;
}

struct expr* parse_additive_expr() {
    struct expr* head;
    struct expr* expr; 
    printf("add ");
    head = parse_multiplicative_expr();
    expr = head;
    while (curr_token->type == '+'
    || curr_token->type == '-') {
        next();
        expr->next = parse_multiplicative_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return expr;
}

struct expr* parse_multiplicative_expr() {
    struct expr* head;
    struct expr* expr; 
    printf("multi ");
    head = parse_unary_expr();
    expr = head;
    while (curr_token->type == '*'
    || curr_token->type == '/') {
        next();
        expr->next = parse_unary_expr();
        expr = expr->next;
    }
    expr->next = NULL;
    return expr;
}

struct expr* parse_unary_expr() {
    printf("unary ");
    switch (curr_token->type) {
        case '+':
            next();
            return parse_unary_expr();
        case '-':
            next();
            return parse_unary_expr();
        case '!':
            next();
            return parse_unary_expr();
        default:
            return parse_primary_expr();
    }
}

struct expr* parse_primary_expr() {
    struct expr* expr;
    printf("primary ");
    switch (curr_token->type) {
        case TOK_ID:
            parse_id();
            if (curr_token->type == '(') {
                struct arg* args = parse_arg_list();
            }
            else if (curr_token->type == '[') {
                next();
                expr = parse_expr();
                expect(']');
            }
            break;
        case '(':
            next();
            expr = parse_expr();
            expect(')');
            break;
        case TOK_NUM:
            expr = malloc(sizeof(struct expr));
            expr->nodes = parse_num();
            break;
        case TOK_STRING:
            expect(TOK_STRING);
            break;
        case '{':
            while (curr_token->type != '}') {
                next();
            }
            expect('}');
            break;
        default:
            break;
    }
    expr = malloc(sizeof(struct expr));
    return expr;
}

struct param* parse_param_list() {
    printf("param_list ");
    expect('(');
    if (!match(')')) {
        parse_id();
        expect(':');
        parse_type();
        while (match(',')) {
            next();
            parse_id();
            expect(':');
            parse_type();
        }
    }
    expect(')');
    return NULL;
}

struct arg* parse_arg_list() {
    printf("arg_list ");
    expect('(');
    if (!match(')')) {
        parse_expr();
        while (curr_token->type == ',') {
            next();
            parse_expr();
        }
    }
    expect(')');
}

struct token* parse_num() {
    printf("num ");
    if (match(TOK_NUM)) {
        next();
    }
}
