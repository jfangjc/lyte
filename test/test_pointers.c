#include "common.h"
#include "parser.h"
#include "lexer.h"
#include "framework.h"
#include <stdio.h>
#include <string.h>

#include "test_utils.h"

void test_pointers_parsing(void) {
    create_temp_file("test_ptr.lt",
                     "fn main(): *s32 { let x: *s32 = &y; return *x; }");
    read_file("test_ptr.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);

    struct fn_decl* fn = prog->fn_decls;
    test_assert_true(fn->type != NULL, "fn->type != NULL", __FILE__, __LINE__);
    test_assert_eq(1, fn->type->kind, __FILE__, __LINE__); // pointer
    test_assert_true(fn->type->base != NULL, "fn->type->base != NULL", __FILE__,
                     __LINE__);
    test_assert_eq(TOK_S32, fn->type->base->primitive, __FILE__, __LINE__);

    struct stmt* body = fn->body;
    struct let_decl* decl = body->stmt.let_decl;
    test_assert_true(decl->type != NULL, "decl->type != NULL", __FILE__,
                     __LINE__);
    test_assert_eq(1, decl->type->kind, __FILE__, __LINE__); // pointer
    test_assert_eq(TOK_S32, decl->type->base->primitive, __FILE__, __LINE__);

    // Check initializer &y
    struct expr* init = decl->value;
    test_assert_eq(EXPR_UNARY, (int)init->type, __FILE__, __LINE__);
    test_assert_eq('&', init->exprs.unary_expr->op, __FILE__, __LINE__);

    remove("test_ptr.lt");
}
