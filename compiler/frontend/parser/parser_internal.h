#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "lexer.h"
#include "parser.h"

extern struct token* curr_token;

void next(void);

void expect(int type);

int match(int type);

struct var_decl* parse_var_decl(void);
struct token* parse_id(void);

char* parse_qualified_name(struct token** first_token);

struct type* parse_type(void);
struct param* parse_param_list(void);
struct expr* parse_initialiser(void);

struct stmt* parse_compound_stmt(void);
struct stmt* parse_stmt(void);

struct expr* parse_expr(void);
struct arg* parse_arg_list(void);

#endif
