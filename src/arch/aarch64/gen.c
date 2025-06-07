#include "gen.h"

#include <stdlib.h>

#include "common.h"
#include "parser.h"
#include "emitter.h"

void gen_fn(struct fn_decl* fn_decl);
void gen_param(struct param* params);

void gen_compound_stmt(struct stmt* stmts);
void gen_stmt(struct stmt* stmt);

void gen(char* bin_name, struct program_ast* program) {
    emitter(bin_name);
    struct fn_decl* fn_decl = program->fn_decls;
    while (fn_decl != NULL) {
        gen_fn(fn_decl);
        fn_decl = fn_decl->next;
    }
    emit_finish();
}

void gen_fn(struct fn_decl* fn_decl) {
    emit_label(fn_decl->name->start_pos, fn_decl->name->length);
    gen_param(fn_decl->params);
    gen_compound_stmt(fn_decl->body);
}

void gen_param(struct param* params) {
    // add param to scope
}

void gen_var(struct var_decl* var_decl) {
    switch (var_decl->type) {
        case TOK_S8:
        case TOK_S16:
        case TOK_S32:
        case TOK_S64:
        case TOK_S128:
        case TOK_U8:
        case TOK_U16:
        case TOK_U32:
        case TOK_U64:
        case TOK_U128:
        case TOK_F32:
        case TOK_F64:
        case TOK_F128:
        default:
    }
}

void gen_compound_stmt(struct stmt* stmts) {
    struct stmt* stmt = stmts;
    while (stmt != NULL) {
        gen_stmt(stmt);
    }
}

void gen_stmt(struct stmt* stmt) {
    if (stmt->stmt.var_decl != NULL) {
    }
    else if (stmt->stmt.addr_decl != NULL) {
    }
    else if (stmt->stmt.if_stmt != NULL) {
    }
    else if (stmt->stmt.for_stmt != NULL) {
    }
    else if (stmt->stmt.break_stmt != NULL) {
    }
    else if (stmt->stmt.continue_stmt != NULL) {
    }
    else if (stmt->stmt.return_stmt != NULL) {
    }
    else {
    }
}

void gen_if() {
    
}

void gen_for() {

}
