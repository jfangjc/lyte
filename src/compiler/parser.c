#include "parser.h"

#include <stdio.h>

#include "scanner.h"
#include "common.h"

struct token* curr_token;

struct fn_decl* parse_fn_decl();
void parse_var_decl();
void parse_init_decl();
void parse_type();
void parse_id();
void parse_compound_stmt();
void parse_stmt();
void parse_if_stmt();
void parse_for_stmt();
void parse_while_stmt();
void parse_break_stmt();
void parse_continue_stmt();
void parse_return_stmt();
void parse_expr_stmt();
void parse_expr();
void parse_assign_expr();
void parse_cond_or_expr();
void parse_cond_and_expr();
void parse_equality_expr();
void parse_rel_expr();
void parse_additive_expr();
void parse_multiplicative_expr();
void parse_unary_expr() ;
void parse_primary_expr();
void parse_param_list();
void parse_arg_list();
void parse_num();

void next() {
    curr_token = next_token();
}

void expect(int type) {
    if (curr_token->type == type) {
        next();
        return;
    }
    printf("\n ERROR: %i \n", type);
    error("wrong type");
    return;
}

int match(int type) {
    return curr_token->type == type;
}

struct program_ast* parse_program() {
    struct program_ast* program = NULL;
    struct fn_decl* fn;
    next();

    while (curr_token != NULL) {
        parse_fn_decl();
        next();
    }

    return program;
}

struct fn_decl* parse_fn_decl() {
    if (curr_token->type != TOK_FN) {
        return NULL;
    }

    printf("fn ");
    expect(TOK_FN);
    parse_id();
    parse_param_list();
    expect(':');
    parse_type();
    parse_compound_stmt();
    return NULL;
}

void parse_var_decl() {
    printf("var ");
    expect(TOK_VAR);
    parse_id();
    if (match('[')) {
        next();
        if (match(TOK_NUM)) {
            parse_num();
        }
        expect(']');
    }
    expect(':');
    parse_type();
    parse_init_decl();
    expect(';');
}

void parse_init_decl() {
    printf("init_decl ");
    if (match('=')) {
        printf("assign ");
        next();
        if (match('{')) {
            next();
            parse_expr();
            while (match(',')) {
                next();
                parse_expr();
            }
            expect('}');
        }
        else {
            parse_expr();
        }
    }
    else {
        parse_expr();
    }
}

void parse_type() {
    printf("type ");
    if (match(TOK_SI8)) {
        
    }
    expect(TOK_SI8);
}

void parse_id() {
    printf("id ");
    expect(TOK_ID);
}

void parse_compound_stmt() {
    printf("compound_stmt ");
    expect('{');
    while (!match('}')) {
        if (match(TOK_VAR)) {
            parse_var_decl();
        }
        else {
            parse_stmt();
        }
    }
    expect('}');
}

void parse_stmt() {
    printf("stmt ");
    switch (curr_token->type) {
        case '{':
            parse_compound_stmt();
            break;
        case TOK_IF:
            parse_if_stmt();
            break;
        case TOK_FOR:
            parse_for_stmt();
            break;
        case TOK_WHILE:
            parse_while_stmt();
            break;
        case TOK_BREAK:
            parse_break_stmt();
            break;
        case TOK_CONTINUE:
            parse_continue_stmt();
            break;
        case TOK_RETURN:
            parse_return_stmt();
            break;
        default:
            parse_expr_stmt();
            break;
    }
}

void parse_if_stmt() {
    printf("if ");
    expect(TOK_IF);
    expect('(');
    parse_expr();
    expect(')');
    parse_compound_stmt();
    if (curr_token->type == TOK_ELSE) {
        next();
        parse_compound_stmt();
    }
}

void parse_for_stmt() {
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
}

void parse_while_stmt() {
    printf("while ");
    expect(TOK_WHILE);
    expect('(');
    parse_expr();
    expect(')');
    parse_compound_stmt();
}

void parse_break_stmt() {
    printf("break ");
    expect(TOK_BREAK);
    expect(';');
}

void parse_continue_stmt() {
    printf("continue ");
    expect(TOK_CONTINUE);
    expect(';');
}

void parse_return_stmt() {
    printf("return ");
    expect(TOK_RETURN);
    if (!match(';')) {
        parse_expr();
    }
    expect(';');
}

void parse_expr_stmt() {
    printf("expr_stmt ");
    if (curr_token->type != ';') {
        parse_expr();
    }
    expect(';');
}

void parse_expr() {
    printf("expr ");
    parse_assign_expr();
}

void parse_assign_expr() {
    printf("assign ");
    parse_cond_or_expr();
    while (curr_token->type == '=') {
        next();
        parse_cond_or_expr();
    }
}

void parse_cond_or_expr() {
    printf("or ");
    parse_cond_and_expr();
    while (curr_token->type == TOK_OROR) {
        next();
        parse_cond_and_expr();
    }
}

void parse_cond_and_expr() {
    printf("and ");
    parse_equality_expr();
    while (curr_token->type == TOK_ANDAND) {
        next();
        parse_equality_expr();
    }
}

void parse_equality_expr() {
    printf("equal ");
    parse_rel_expr();
    while (curr_token->type == TOK_EQEQ
    || curr_token->type == TOK_NOTEQ) {
        next();
        parse_rel_expr();
    }
}

void parse_rel_expr() {
    printf("rel ");
    parse_additive_expr();
    while (curr_token->type == '<'
    || curr_token->type == TOK_LTEQ
    || curr_token->type == '>'
    || curr_token->type == TOK_GTEQ) {
        next();
        parse_additive_expr();
    }
}

void parse_additive_expr() {
    printf("add ");
    parse_multiplicative_expr();
    while (curr_token->type == '+'
    || curr_token->type == '-') {
        next();
        parse_multiplicative_expr();
    }
}

void parse_multiplicative_expr() {
    printf("multi ");
    parse_unary_expr();
    while (curr_token->type == '*'
    || curr_token->type == '/') {
        next();
        parse_unary_expr();
    }
}

void parse_unary_expr() {
    printf("unary ");
    switch (curr_token->type) {
        case '+':
            next();
            parse_unary_expr();
            break;
        case '-':
            next();
            parse_unary_expr();
            break;
        case '!':
            next();
            parse_unary_expr();
            break;
        default:
            parse_primary_expr();
            break;
    }
}

void parse_primary_expr() {
    printf("primary ");
    switch (curr_token->type) {
        case TOK_ID:
            parse_id();
            if (curr_token->type == '(') {
                parse_arg_list();
            }
            else if (curr_token->type == '[') {
                next();
                parse_expr();
                expect(']');
            }
            break;
        case '(':
            next();
            parse_expr();
            expect(')');
            break;
        case TOK_NUM:
            parse_num();
            break;
        case TOK_STRING:
            expect(TOK_STRING);
            break;
        default:
            break;
    }
}

void parse_param_list() {
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
}

void parse_arg_list() {
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

void parse_num() {
    printf("num ");
    if (match(TOK_NUM)) {
        next();
    }
}
