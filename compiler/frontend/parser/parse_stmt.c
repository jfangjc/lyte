#include "common.h"
#include "parser_internal.h"

#include <stdlib.h>

static struct if_stmt* parse_if_stmt(void);
static struct for_stmt* parse_for_stmt(void);
static struct unsafe_stmt* parse_unsafe_stmt(void);
static void parse_break_stmt(void);
static void parse_continue_stmt(void);
static struct return_stmt* parse_return_stmt(void);
static struct expr_stmt* parse_expr_stmt(void);

// Compound statement
struct stmt* parse_compound_stmt(void) {
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

// Decide which kind of statement to parse
struct stmt* parse_stmt(void) {
    struct stmt* stmt = malloc(sizeof(struct stmt));

    switch (curr_token->type) {
    case TOK_VAR:
    case TOK_CONST:
        stmt->type = STMT_VAR;
        stmt->stmt.var_decl = parse_var_decl();
        break;

    case TOK_IF:
        stmt->type = STMT_IF;
        stmt->stmt.if_stmt = parse_if_stmt();
        break;

    case TOK_FOR:
        stmt->type = STMT_FOR;
        stmt->stmt.for_stmt = parse_for_stmt();
        break;

    case TOK_UNSAFE:
        stmt->type = STMT_UNSAFE;
        stmt->stmt.unsafe_stmt = parse_unsafe_stmt();
        break;

    case TOK_BREAK:
        stmt->type = STMT_BREAK;
        parse_break_stmt();
        break;

    case TOK_CONTINUE:
        stmt->type = STMT_CONTINUE;
        parse_continue_stmt();
        break;

    case TOK_RETURN:
        stmt->type = STMT_RETURN;
        stmt->stmt.return_stmt = parse_return_stmt();
        break;

    default:
        stmt->type = STMT_EXPR;
        stmt->stmt.expr_stmt = parse_expr_stmt();
    }
    return stmt;
}

// Individual statement parsers
static struct if_stmt* parse_if_stmt(void) {
    struct if_stmt* if_stmt = malloc(sizeof(struct if_stmt));

    expect(TOK_IF);
    expect('(');
    if_stmt->if_cond = parse_expr();
    expect(')');
    if_stmt->if_body = parse_compound_stmt();

    struct elseif_stmt* elseif = NULL;
    struct elseif_stmt* head = NULL;
    if_stmt->elseif_stmt = NULL;
    if_stmt->else_body = NULL;

    while (match(TOK_ELSE)) {
        next();
        if (match(TOK_IF)) {
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
            elseif->next = NULL;
        }
        else {
            if_stmt->elseif_stmt = head;
            if_stmt->else_body = parse_compound_stmt();
            return if_stmt;
        }
    }

    if_stmt->elseif_stmt = head;
    return if_stmt;
}

static struct for_stmt* parse_for_stmt(void) {
    struct for_stmt* for_stmt = malloc(sizeof(struct for_stmt));

    expect(TOK_FOR);
    for_stmt->init = NULL;
    for_stmt->cond = NULL;
    for_stmt->step = NULL;

    if (!match(';')) {
        if (match(TOK_VAR) || match(TOK_CONST)) {
            for_stmt->init = malloc(sizeof(struct stmt));
            for_stmt->init->type = STMT_VAR;
            for_stmt->init->stmt.var_decl = parse_var_decl();
            for_stmt->init->next = NULL;
        }
        else {
            for_stmt->init = malloc(sizeof(struct stmt));
            for_stmt->init->type = STMT_EXPR;
            for_stmt->init->stmt.expr_stmt = parse_expr_stmt();
            for_stmt->init->next = NULL;
        }
    }
    else {
        expect(';');
    }

    if (!match(';')) {
        for_stmt->cond = parse_expr();
    }
    expect(';');

    if (!match('{')) {
        for_stmt->step = parse_expr();
    }
    for_stmt->body = parse_compound_stmt();
    return for_stmt;
}

static struct unsafe_stmt* parse_unsafe_stmt(void) {
    struct unsafe_stmt* unsafe_stmt = malloc(sizeof(struct unsafe_stmt));
    expect(TOK_UNSAFE);
    unsafe_stmt->body = parse_compound_stmt();
    return unsafe_stmt;
}

static void parse_break_stmt(void) {
    expect(TOK_BREAK);
    expect(';');
}

static void parse_continue_stmt(void) {
    expect(TOK_CONTINUE);
    expect(';');
}

static struct return_stmt* parse_return_stmt(void) {
    struct return_stmt* return_stmt = malloc(sizeof(struct return_stmt));
    return_stmt->expr = NULL;
    expect(TOK_RETURN);
    if (!match(';')) {
        return_stmt->expr = parse_expr();
    }
    expect(';');
    return return_stmt;
}

static struct expr_stmt* parse_expr_stmt(void) {
    struct expr_stmt* expr_stmt = malloc(sizeof(struct expr_stmt));
    expr_stmt->expr = NULL;
    if (curr_token->type != ';') {
        expr_stmt->expr = parse_expr();
    }
    expect(';');
    return expr_stmt;
}
