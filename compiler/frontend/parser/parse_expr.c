#include "common.h"
#include "error.h"
#include "parser_internal.h"

#include <stdlib.h>
#include <string.h>

static struct expr* parse_assignment(void);
static struct expr* parse_logic_or(void);
static struct expr* parse_logic_and(void);
static struct expr* parse_equality(void);
static struct expr* parse_relational(void);
static struct expr* parse_term(void);
static struct expr* parse_factor(void);
static struct expr* parse_unary(void);
static struct expr* parse_primary(void);

struct expr* parse_expr(void) { return parse_assignment(); }

static struct expr* parse_assignment(void) {
    struct expr* expr = parse_logic_or();

    if (match(TOK_ASSIGN) || match(TOK_ADD_ASSIGN) || match(TOK_SUB_ASSIGN) || match(TOK_MUL_ASSIGN) ||
        match(TOK_DIV_ASSIGN)) {

        if (expr->type != EXPR_ID) {
            error("Invalid assignment target");
        }

        struct expr* assign_expr = malloc(sizeof(struct expr));
        assign_expr->type = EXPR_ASSIGN;
        assign_expr->exprs.assign_expr = malloc(sizeof(struct assign_expr));
        assign_expr->exprs.assign_expr->id = expr->exprs.id;
        assign_expr->exprs.assign_expr->op = curr_token->type;
        next();

        assign_expr->exprs.assign_expr->value = parse_assignment();
        free(expr);
        return assign_expr;
    }
    return expr;
}

// Logical OR (||)
static struct expr* parse_logic_or(void) {
    struct expr* expr = parse_logic_and();
    while (match(TOK_OR)) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type = EXPR_BOOLEAN;
        new_expr->exprs.boolean_expr = malloc(sizeof(struct boolean_expr));
        new_expr->exprs.boolean_expr->left = expr;
        new_expr->exprs.boolean_expr->op = curr_token->type;
        next();
        new_expr->exprs.boolean_expr->right = parse_logic_and();
        expr = new_expr;
    }
    return expr;
}

// Logical AND (&&)
static struct expr* parse_logic_and(void) {
    struct expr* expr = parse_equality();
    while (match(TOK_AND)) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type = EXPR_BOOLEAN;
        new_expr->exprs.boolean_expr = malloc(sizeof(struct boolean_expr));
        new_expr->exprs.boolean_expr->left = expr;
        new_expr->exprs.boolean_expr->op = curr_token->type;
        next();
        new_expr->exprs.boolean_expr->right = parse_equality();
        expr = new_expr;
    }
    return expr;
}

// Equality (==, !=)
static struct expr* parse_equality(void) {
    struct expr* expr = parse_relational();
    while (match(TOK_EQEQ) || match(TOK_NOTEQ)) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type = EXPR_EQUALITY;
        new_expr->exprs.equality_expr = malloc(sizeof(struct equality_expr));
        new_expr->exprs.equality_expr->left = expr;
        new_expr->exprs.equality_expr->op = curr_token->type;
        next();
        new_expr->exprs.equality_expr->right = parse_relational();
        expr = new_expr;
    }
    return expr;
}

// Relational (<, >, <=, >=)
static struct expr* parse_relational(void) {
    struct expr* expr = parse_term();
    while (match('>') || match('<') || match(TOK_GTEQ) || match(TOK_LTEQ)) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type = EXPR_EQUALITY; /* reuse equality_expr for relational */
        new_expr->exprs.equality_expr = malloc(sizeof(struct equality_expr));
        new_expr->exprs.equality_expr->left = expr;
        new_expr->exprs.equality_expr->op = curr_token->type;
        next();
        new_expr->exprs.equality_expr->right = parse_term();
        expr = new_expr;
    }
    return expr;
}

// Term (+, -)
static struct expr* parse_term(void) {
    struct expr* expr = parse_factor();
    while (match('+') || match('-')) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type = EXPR_ARITH;
        new_expr->exprs.arith_expr = malloc(sizeof(struct arith_expr));
        new_expr->exprs.arith_expr->left = expr;
        new_expr->exprs.arith_expr->op = curr_token->type;
        next();
        new_expr->exprs.arith_expr->right = parse_factor();
        expr = new_expr;
    }
    return expr;
}

// Factor (*, /)
static struct expr* parse_factor(void) {
    struct expr* expr = parse_unary();
    while (match('*') || match('/')) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type = EXPR_ARITH;
        new_expr->exprs.arith_expr = malloc(sizeof(struct arith_expr));
        new_expr->exprs.arith_expr->left = expr;
        new_expr->exprs.arith_expr->op = curr_token->type;
        next();
        new_expr->exprs.arith_expr->right = parse_unary();
        expr = new_expr;
    }
    return expr;
}

// Unary ( !, -, *, & )
static struct expr* parse_unary(void) {
    if (match('!') || match('-') || match('*') || match('&')) {
        struct expr* expr = malloc(sizeof(struct expr));
        expr->type = EXPR_UNARY;
        expr->exprs.unary_expr = malloc(sizeof(struct unary_expr));
        expr->exprs.unary_expr->op = curr_token->type;
        next();
        expr->exprs.unary_expr->expr = parse_unary();
        return expr;
    }
    return parse_primary();
}

// Primary expression
static struct expr* parse_primary(void) {
    struct expr* expr = malloc(sizeof(struct expr));

    if (match(TOK_INT) || match(TOK_FLOAT) || match(TOK_STRING)) {
        expr->type = EXPR_VALUE;
        expr->exprs.value_expr = curr_token;
        next();
    }
    else if (match(TOK_ID)) {
        struct token* id = NULL;
        char* name = parse_qualified_name(&id);
        while (match('[')) {
            next();
            parse_expr();
            expect(']');
        }
        if (match('(')) { // function call
            next();
            expr->type = EXPR_CALL;
            expr->exprs.call_expr = malloc(sizeof(struct call_expr));
            expr->exprs.call_expr->id = id;
            expr->exprs.call_expr->name = name;
            if (curr_token->type != ')') {
                expr->exprs.call_expr->args = parse_arg_list();
            }
            else {
                expr->exprs.call_expr->args = NULL;
            }
            expect(')');
        }
        else {
            expr->type = EXPR_ID;
            expr->exprs.id = id;
            free(name);
        }
    }
    else if (match('(')) {
        free(expr);
        next();
        expr = parse_expr();
        expect(')');
    }
    else {
        error("Expected expression");
    }
    while (match('!')) {
        next();
    }
    return expr;
}

// Argument list
struct arg* parse_arg_list(void) {
    struct arg* arg = malloc(sizeof(struct arg));
    struct arg* head = arg;
    if (match(TOK_ID) && curr_token->length == 3 && strncmp(curr_token->start_pos, "out", 3) == 0) {
        next();
    }
    arg->value = parse_expr();

    while (!match(')')) {
        arg->next = malloc(sizeof(struct arg));
        arg = arg->next;
        expect(',');
        if (match(TOK_ID) && curr_token->length == 3 && strncmp(curr_token->start_pos, "out", 3) == 0) {
            next();
        }
        arg->value = parse_expr();
    }
    arg->next = NULL;
    return head;
}
