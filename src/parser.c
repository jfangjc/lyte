#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "error.h"
#include "scanner.h"

struct token* curr_token;

struct fn_decl* parse_fn_decl(void);
struct let_decl* parse_let_decl(void);
struct addr_decl* parse_addr_decl(void);

struct param* parse_param_list(void);
struct expr* parse_initialiser(void);

int parse_type(void);

struct token* parse_id(void);

struct stmt* parse_compound_stmt(void);
struct stmt* parse_stmt(void);

struct if_stmt* parse_if_stmt(void);
struct for_stmt* parse_for_stmt(void);
struct break_stmt* parse_break_stmt(void);
struct continue_stmt* parse_continue_stmt(void);
struct return_stmt* parse_return_stmt(void);
struct expr_stmt* parse_expr_stmt(void);

struct expr* parse_expr(void);

struct expr* parse_assignment(void);
struct expr* parse_logic_or(void);
struct expr* parse_logic_and(void);
struct expr* parse_equality(void);
struct expr* parse_relational(void);
struct expr* parse_term(void);
struct expr* parse_factor(void);
struct expr* parse_unary(void);
struct expr* parse_primary(void);

struct arg* parse_arg_list(void);

int parse_int(void);
double parse_float(void);

void next(void) { curr_token = next_token(); }

void expect(int type) {
    if (curr_token == NULL) {
        char error_str[256];
        snprintf(error_str, 256, "\n ERROR: expect %c, get NULL \n", type);
        error(error_str);
        return;
    }
    if (curr_token->type == type) {
        next();
        return;
    }
    char error_str[256];
    snprintf(error_str, 256, "\n ERROR: expect %c, get %c \n", type, curr_token->start_pos[0]);
    error(error_str);
    return;
}

int match(int type) { return curr_token->type == type; }

struct program_ast* parse_program(void) {
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

struct fn_decl* parse_fn_decl(void) {
    expect(TOK_FN);

    struct fn_decl* fn = malloc(sizeof(struct fn_decl));

    fn->name = parse_id();

    fn->params = parse_param_list();

    expect(':');

    fn->type = parse_type();

    fn->body = parse_compound_stmt();

    return fn;
}

struct let_decl* parse_let_decl(void) {
    struct let_decl* var = malloc(sizeof(struct let_decl));

    expect(TOK_LET);

    if (match(TOK_MUT)) {
        var->mut = 1;
        next();
    }
    else {
        var->mut = 0;
    }

    var->name = parse_id();

    if (match(':')) {
        next();
        var->type = parse_type();
    }
    else {
        var->type = 0; // 0 for auto/unknown type, or handle accordingly
    }

    var->value = parse_initialiser();

    expect(';');
    return var;
}

struct addr_decl* parse_addr_decl(void) {
    struct addr_decl* addr = malloc(sizeof(struct addr_decl));

    expect(TOK_ADDR);

    addr->name = parse_id();

    expect(':');

    addr->type = parse_type();

    addr->value = parse_initialiser();

    expect(';');
    return addr;
}

struct param* parse_param_list(void) {
    expect('(');

    if (!match(')')) {
        struct param* param = malloc(sizeof(struct param));
        struct param* head = param;

        param->name = parse_id();
        expect(':');
        if (match(TOK_MUT)) {
            param->mut = 1;
            next();
        }
        else {
            param->mut = 0;
        }
        param->type = parse_type();

        while (match(',')) {
            param->next = malloc(sizeof(struct param));
            param = param->next;
            next();

            param->name = parse_id();
            expect(':');
            if (match(TOK_MUT)) {
                param->mut = 1;
                next();
            }
            else {
                param->mut = 0;
            }
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

struct expr* parse_initialiser(void) {
    if (match(TOK_ASSIGN)) {
        next();
        return parse_expr();
    }

    return NULL;
}

int parse_type(void) {
    if (curr_token->type < TOK_S8 || curr_token->type > TOK_F128) {
        printf("< %i >", curr_token->type);
        error("Unrecognised type");
    }
    int type = curr_token->type;
    next();
    return type;
}

struct token* parse_id(void) {
    struct token* token = curr_token;
    expect(TOK_ID);
    return token;
}

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

struct stmt* parse_stmt(void) {
    struct stmt* stmt = malloc(sizeof(struct stmt));

    switch (curr_token->type) {
    case TOK_LET:
        stmt->type = STMT_LET;
        stmt->stmt.let_decl = parse_let_decl();
        break;
    case TOK_ADDR:
        stmt->type = STMT_ADDR;
        stmt->stmt.addr_decl = parse_addr_decl();
        break;
    case TOK_IF:
        stmt->type = STMT_IF;
        stmt->stmt.if_stmt = parse_if_stmt();
        break;
    case TOK_FOR:
        stmt->type = STMT_FOR;
        stmt->stmt.for_stmt = parse_for_stmt();
        break;
    case TOK_BREAK:
        stmt->type = STMT_BREAK;
        stmt->stmt.break_stmt = parse_break_stmt();
        break;
    case TOK_CONTINUE:
        stmt->type = STMT_CONTINUE;
        stmt->stmt.continue_stmt = parse_continue_stmt();
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

struct if_stmt* parse_if_stmt(void) {
    struct if_stmt* if_stmt = malloc(sizeof(struct if_stmt));

    expect(TOK_IF);

    expect('(');

    if_stmt->if_cond = parse_expr();

    expect(')');

    if_stmt->if_body = parse_compound_stmt();

    struct elseif_stmt* elseif = NULL;
    struct elseif_stmt* head = NULL;

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

struct for_stmt* parse_for_stmt(void) {
    struct for_stmt* for_stmt = malloc(sizeof(struct for_stmt));

    expect(TOK_FOR);
    expect('(');

    for_stmt->cond = parse_expr();

    expect(')');

    for_stmt->body = parse_compound_stmt();

    return for_stmt;
}

struct break_stmt* parse_break_stmt(void) {
    struct break_stmt* break_stmt = malloc(sizeof(struct break_stmt));
    expect(TOK_BREAK);
    expect(';');
    return break_stmt;
}

struct continue_stmt* parse_continue_stmt(void) {
    struct continue_stmt* continue_stmt = malloc(sizeof(struct continue_stmt));
    expect(TOK_CONTINUE);
    expect(';');
    return continue_stmt;
}

struct return_stmt* parse_return_stmt(void) {
    struct return_stmt* return_stmt = malloc(sizeof(struct return_stmt));
    expect(TOK_RETURN);
    if (!match(';')) {
        return_stmt->expr = parse_expr();
    }
    expect(';');
    return return_stmt;
}

struct expr_stmt* parse_expr_stmt(void) {
    struct expr_stmt* expr_stmt = malloc(sizeof(struct expr_stmt));
    if (curr_token->type != ';') {
        expr_stmt->expr = parse_expr();
    }
    expect(';');
    return expr_stmt;
}


// Start with the lowest precedence
struct expr* parse_expr(void) { return parse_assignment(); }

// Handle assignment expressions (right-associative)
struct expr* parse_assignment(void) {
    struct expr* expr = parse_logic_or();

    if (match(TOK_ASSIGN) || match(TOK_ADD_ASSIGN) || match(TOK_SUB_ASSIGN) ||
        match(TOK_MUL_ASSIGN) || match(TOK_DIV_ASSIGN)) {

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

struct expr* parse_logic_or(void) {
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

struct expr* parse_logic_and(void) {
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

struct expr* parse_equality(void) {
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

struct expr* parse_relational(void) {
    struct expr* expr = parse_term();
    while (match('>') || match('<') || match(TOK_GTEQ) || match(TOK_LTEQ)) {
        struct expr* new_expr = malloc(sizeof(struct expr));
        new_expr->type =
            EXPR_EQUALITY; // Reuse equality_expr for relational ops
        new_expr->exprs.equality_expr = malloc(sizeof(struct equality_expr));
        new_expr->exprs.equality_expr->left = expr;
        new_expr->exprs.equality_expr->op = curr_token->type;
        next();
        new_expr->exprs.equality_expr->right = parse_term();
        expr = new_expr;
    }
    return expr;
}

struct expr* parse_term(void) {
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

struct expr* parse_factor(void) {
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

struct expr* parse_unary(void) {
    if (match('!') || match('-')) {
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

struct expr* parse_primary(void) {
    struct expr* expr = malloc(sizeof(struct expr));

    if (match(TOK_INT) || match(TOK_FLOAT) || match(TOK_STRING)) {
        expr->type = EXPR_VALUE;
        expr->exprs.value_expr = curr_token;
        next();
    }
    else if (match(TOK_ID)) {
        struct token* id = parse_id();
        if (match('(')) { // Function call
            next();
            expr->type = EXPR_CALL;
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
            expr->type = EXPR_ID;
            expr->exprs.id = id;
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
    return expr;
}

struct arg* parse_arg_list(void) {
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

int parse_int(void) {
    int num = 0;
    if (match(TOK_INT)) {
        for (int i = 0; i < curr_token->length; i++) {
            num *= 10;
            num += curr_token->start_pos[i] - '0';
        }
    }

    next();
    return num;
}

double parse_float(void) {
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

    next();
    return num;
}
