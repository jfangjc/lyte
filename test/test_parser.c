#include "../src/compiler/common.h"
#include "../src/compiler/parser.h"
#include "../src/compiler/scanner.h"
#include "framework.h"

#include "test_utils.h"

TEST(parser_fn_decl) {
    create_temp_file("test_fn.lt", "fn main(): s32 { return 0; };");
    read_file("test_fn.lt");

    struct program_ast* prog = parse_program();
    ASSERT_TRUE(prog != NULL);
    ASSERT_TRUE(prog->fn_decls != NULL);

    struct fn_decl* fn = prog->fn_decls;
    ASSERT_TRUE(fn->name != NULL);
    ASSERT_TRUE(strncmp(fn->name->start_pos, "main", 4) == 0);
    ASSERT_EQ(TOK_S32, fn->type);

    remove("test_fn.lt");
}

TEST(parser_var_decl) {
    create_temp_file("test_var.lt", "fn test(): s32 { let x: s32 = 10; };");
    read_file("test_var.lt");

    struct program_ast* prog = parse_program();
    ASSERT_TRUE(prog != NULL);
    ASSERT_TRUE(prog->fn_decls != NULL);

    struct fn_decl* fn = prog->fn_decls;
    struct stmt* body = fn->body;
    ASSERT_TRUE(body != NULL);

    ASSERT_TRUE(body->stmt.let_decl != NULL);
    struct let_decl* var = body->stmt.let_decl;
    ASSERT_TRUE(strncmp(var->name->start_pos, "x", 1) == 0);
    ASSERT_EQ(TOK_S32, var->type);

    remove("test_var.lt");
}
