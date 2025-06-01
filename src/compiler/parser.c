#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "common.h"
#include "error.h"

struct token* curr_token;

struct fn_decl* parse_fn_decl();
struct var_decl* parse_var_decl();
struct addr_decl* parse_addr_decl();

struct param* parse_param_list();
struct expr* parse_initialiser();

int parse_type();

struct token* parse_id();

struct stmt* parse_compound_stmt();
struct stmt* parse_stmt();

struct if_stmt* parse_if_stmt();
struct for_stmt* parse_for_stmt();
struct break_stmt* parse_break_stmt();
struct continue_stmt* parse_continue_stmt();
struct return_stmt* parse_return_stmt();
struct expr_stmt* parse_expr_stmt();

struct expr* parse_expr();
struct arg* parse_arg_list();

int parse_int();
double parse_float();

void next() {
    curr_token = next_token();
}

void expect(int type) {
    if (curr_token->type == type) {
        next();
        return;
    }
    printf("\n ERROR: expect %i : get %c \n", type, curr_token->start_pos[0]);
    error("wrong type");
    return;
}

int match(int type) {
    return curr_token->type == type;
}

struct program_ast* parse_program() {
    struct program_ast* program = malloc(sizeof(struct program_ast));
    struct fn_decl** fn = &(program->fn_decls);
    curr_token = next_token();

    while (curr_token != NULL) {
        (*fn) = parse_fn_decl();
        fn = &((*fn)->next);
    }
    (*fn) = NULL;

    return program;
}

struct fn_decl* parse_fn_decl() {
    printf("fn ");
    expect(TOK_FN);

    struct fn_decl* fn = malloc(sizeof(struct fn_decl));

    fn->name = parse_id();

    fn->params = parse_param_list();

    expect(':');

    fn->type = parse_type();

    fn->body = parse_compound_stmt();

    expect(';');
    return fn;
}

struct var_decl* parse_var_decl() {
    struct var_decl* var = malloc(sizeof(struct var_decl));
    printf("var ");

    expect(TOK_VAR);

    var->name = parse_id();

    expect(':');

    var->type = parse_type();

    var->value = parse_initialiser();

    expect(';');
    return var;
}

struct addr_decl* parse_addr_decl() {
    struct addr_decl* addr = malloc(sizeof(struct addr_decl));
    printf("addr ");

    expect(TOK_ADDR);

    addr->name = parse_id();

    expect(':');

    addr->type = parse_type();

    addr->value = parse_initialiser();

    expect(';');
    return addr;
}

struct param* parse_param_list() {
    printf("param_list ");
    expect('(');

    if (!match(')')) {
        struct param* param = malloc(sizeof(struct param));
        struct param* head = param;

        param->name = parse_id();
        expect(':');
        param->type = parse_type();

        while (match(',')) {
            param->next = malloc(sizeof(struct param));
            param = param->next;
            next();

            param->name = parse_id();
            expect(':');
            param->type = parse_type();
        }
        param->next = NULL;
        next();
        return head;
    }
    else {
        next();
        return NULL;
    }
}

struct expr* parse_initialiser() {
    printf("init_decl ");

    if (match(TOK_ASSIGN)) {
        next();
        return parse_expr();
    }

    return NULL;
}

int parse_type() {
    printf("type ");
    if (curr_token->type < TOK_S8 || curr_token->type > TOK_F128) {
        printf("< %i >", curr_token->type);
        error("Unrecognised type");
    }
    int type = curr_token->type;
    next();
    return type;
}

struct token* parse_id() {
    printf("id ");
    struct token* token = curr_token;
    expect(TOK_ID);
    return token;
}

struct stmt* parse_compound_stmt() {
    printf("compound_stmt ");
    expect('{');

    if (match('}')) {
        next();
        return NULL;
    }

    struct stmt* head = parse_stmt();
    struct stmt* stmt = head;

    while (!match('}')) {
        stmt->next = parse_stmt();
        stmt = stmt->next;
    }

    stmt->next = NULL;

    expect('}');
    return head;
}

struct stmt* parse_stmt() {
    printf("stmt ");
    struct stmt* stmt = malloc(sizeof(struct stmt));
   
    switch (curr_token->type) {
        case TOK_VAR:
            stmt->stmt.var_decl = parse_var_decl();
            break;
        case TOK_ADDR:
            stmt->stmt.addr_decl = parse_addr_decl();
            break;
        case TOK_IF:
            stmt->stmt.if_stmt = parse_if_stmt();
            break;
        case TOK_FOR:
            stmt->stmt.for_stmt = parse_for_stmt();
            break;
        case TOK_BREAK:
            stmt->stmt.break_stmt = parse_break_stmt();
            break;
        case TOK_CONTINUE:
            stmt->stmt.continue_stmt = parse_continue_stmt();
            break;
        case TOK_RETURN:
            stmt->stmt.return_stmt = parse_return_stmt();
            break;
        default:
            stmt->stmt.expr_stmt = parse_expr_stmt();
    }
    return stmt;
}

struct if_stmt* parse_if_stmt() {
    printf("if ");
    struct if_stmt* if_stmt = malloc(sizeof(struct if_stmt));

    expect(TOK_IF);

    expect('(');

    if_stmt->if_cond = parse_expr();

    expect(')');

    if_stmt->if_body = parse_compound_stmt();

    int else_count = 0;
    struct elseif_stmt* elseif = NULL;
    struct elseif_stmt* head = NULL;

    while (match(TOK_ELSE)) {
        next();
        if (match(TOK_IF)) {
            printf("elseif ");
            if (elseif == NULL) {
                elseif = malloc(sizeof(struct elseif_stmt));
                head = elseif;
            }
            else {
                elseif->next = malloc(sizeof(struct elseif_stmt));
                elseif = elseif->next;
            }

            next();
            expect('(');
            elseif->cond = parse_expr();
            expect(')');
            elseif->body = parse_compound_stmt();
        }
        else {
            printf("else ");
            if_stmt->elseif_stmt = head;
            if_stmt->else_body = parse_compound_stmt();
            return if_stmt;
        }
    }
    
    if_stmt->elseif_stmt = head;
    return if_stmt;
}

struct for_stmt* parse_for_stmt() {
    printf("for ");
    struct for_stmt* for_stmt = malloc(sizeof(struct for_stmt));
    
    expect(TOK_FOR);
    expect('(');

    for_stmt->cond = parse_expr();

    expect(')');

    for_stmt->body = parse_compound_stmt();

    return for_stmt;
}

struct break_stmt* parse_break_stmt() {
    printf("break ");
    struct break_stmt* break_stmt = malloc(sizeof(struct break_stmt));
    
    expect(TOK_BREAK);
    expect(';');
    return break_stmt;
}

struct continue_stmt* parse_continue_stmt() {
    printf("continue ");
    struct continue_stmt* continue_stmt = malloc(sizeof(struct continue_stmt));
    
    expect(TOK_CONTINUE);
    expect(';');
    return continue_stmt;
}

struct return_stmt* parse_return_stmt() {
    printf("return ");
    struct return_stmt* return_stmt = malloc(sizeof(struct return_stmt));
    
    expect(TOK_RETURN);
    if (!match(';')) {
        return_stmt->expr = parse_expr();
    }
    expect(';');
    return return_stmt;
}

struct expr_stmt* parse_expr_stmt() {
    printf("expr_stmt ");
    struct expr_stmt* expr_stmt = malloc(sizeof(struct expr_stmt));
    if (curr_token->type != ';') {
        expr_stmt->expr = parse_expr();
    }
    expect(';');
    return expr_stmt;
}

struct expr* parse_expr() {
    printf("expr ");
    struct expr* expr = malloc(sizeof(struct expr));
    if (match(TOK_ID)) {
        struct token* id = parse_id();
        if (match(TOK_ASSIGN)) {
            printf("assign ");
            expr->exprs.assign_expr = malloc(sizeof(struct assign_expr));
            expr->exprs.assign_expr->id = id;
            expect(TOK_ASSIGN);
            expr->exprs.assign_expr->value = parse_expr();
        }
        else if (match(TOK_OR) || match(TOK_AND)) {
            printf("boolean ");
            expr->exprs.boolean_expr = malloc(sizeof(struct boolean_expr));
            expr->exprs.boolean_expr->left = id;
            expr->exprs.boolean_expr->op = curr_token->type;
            next();
            expr->exprs.boolean_expr->right = parse_expr();
        }
        else if (match(TOK_LTEQ) || match(TOK_GTEQ) || match(TOK_EQEQ)
        || match('>') || match('<') ) {
            printf("equality ");
            expr->exprs.equality_expr = malloc(sizeof(struct equality_expr));
            expr->exprs.equality_expr->left = id;
            expr->exprs.equality_expr->op = curr_token->type;
            next();
            expr->exprs.equality_expr->right = parse_expr();
        }
        else if (match('+') || match('-') || match('*') || match('/')) {
            printf("arith ");
            expr->exprs.arith_expr = malloc(sizeof(struct arith_expr));
            expr->exprs.arith_expr->left = id;
            expr->exprs.arith_expr->op = curr_token->type;
            next();
            expr->exprs.arith_expr->right = parse_expr();
        }
        else if (match('(')) {
            printf("call ");
            next();
            expr->exprs.call_expr = malloc(sizeof(struct call_expr));
            expr->exprs.call_expr->id = id;
            if (curr_token->type != ')') {
                expr->exprs.call_expr->args = parse_arg_list();
            }
            else {
                expr->exprs.call_expr->args = NULL;
            }
            expect(')');
        }
        else {
            printf("ident ");
            expr->exprs.id = id;
        }
    }
    else if (match('+') || match('-') || match('!')) {
        printf("unary ");
        expr->exprs.unary_expr = malloc(sizeof(struct unary_expr));
        expr->exprs.unary_expr->op = curr_token->type;
        next();
        expr->exprs.unary_expr->expr = parse_expr();
    }
    else if (match('(')) {
        next();
        expr = parse_expr();
        expect(')');
    }
    else if (match(TOK_INT) || match(TOK_FLOAT) || match(TOK_STRING)) {
        printf("num ");
        expr->exprs.value_expr = curr_token;
        next();
    }
    else {
        printf("unknow ");
        return NULL;
    }
    return expr;
}

struct arg* parse_arg_list() {
    struct arg* arg = malloc(sizeof(struct arg));
    struct arg* head = arg;
    arg->value = parse_expr();
    while (!match(')')) {
        arg->next = malloc(sizeof(struct arg));
        arg = arg->next;
        expect(',');
        arg->value = parse_expr();
    }
    arg->next = NULL;
    return head;
}


int parse_int() {
    int num = 0;
    if (match(TOK_INT)) {
        for (int i = 0; i < curr_token->length; i++) {
            num *= 10;
            num += curr_token->start_pos[i] - '0';
        }
    }
    printf("%i ", num);
    next();
    return num;
}

double parse_float() {
    double num = 0;
    int dec = 0;
    if (match(TOK_FLOAT)) {
        for (int i = 0; i < curr_token->length; i++) {
            if (curr_token->start_pos[i] == '.') {
                dec = 10;
                continue;
            }
            if (dec == 0) {
                num *= 10;
                num += curr_token->start_pos[i] - '0';
            }
            else {
                num += (double)(curr_token->start_pos[i] - '0') / dec;
                dec *= 10;
            }
        }
    }
    printf("%lf ", num);
    next();
    return num;
}
