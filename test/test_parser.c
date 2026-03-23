#include "common.h"
#include "parser.h"
#include "lexer.h"
#include "framework.h"
#include <stdio.h>
#include <string.h>

#include "test_utils.h"

void test_parser_fn_decl(void) {
    create_temp_file("test_fn.lt", "fn main(): s32 { return 0; }");
    read_file("test_fn.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);
    test_assert_true(prog->fn_decls != NULL, "prog->fn_decls != NULL", __FILE__,
                     __LINE__);

    struct fn_decl* fn = prog->fn_decls;
    test_assert_true(fn->name != NULL, "fn->name != NULL", __FILE__, __LINE__);
    test_assert_true(strncmp(fn->name->start_pos, "main", 4) == 0,
                     "strncmp(fn->name->start_pos, \"main\", 4) == 0", __FILE__,
                     __LINE__);
    test_assert_true(fn->type != NULL, "fn->type != NULL", __FILE__, __LINE__);
    test_assert_eq(0, fn->type->kind, __FILE__, __LINE__);
    test_assert_eq(TOK_S32, fn->type->primitive, __FILE__, __LINE__);

    remove("test_fn.lt");
}

void test_parser_var_decl(void) {
    create_temp_file("test_var.lt", "fn test(): s32 { let x: s32 = 10; }");
    read_file("test_var.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);
    test_assert_true(prog->fn_decls != NULL, "prog->fn_decls != NULL", __FILE__,
                     __LINE__);

    struct fn_decl* fn = prog->fn_decls;
    struct stmt* body = fn->body;
    test_assert_true(body != NULL, "body != NULL", __FILE__, __LINE__);

    test_assert_true(body->stmt.let_decl != NULL, "body->stmt.let_decl != NULL",
                     __FILE__, __LINE__);
    struct let_decl* var = body->stmt.let_decl;
    test_assert_true(strncmp(var->name->start_pos, "x", 1) == 0,
                     "strncmp(var->name->start_pos, \"x\", 1) == 0", __FILE__,
                     __LINE__);
    test_assert_true(var->type != NULL, "var->type != NULL", __FILE__,
                     __LINE__);
    test_assert_eq(0, var->type->kind, __FILE__, __LINE__);
    test_assert_eq(TOK_S32, var->type->primitive, __FILE__, __LINE__);

    remove("test_var.lt");
}
