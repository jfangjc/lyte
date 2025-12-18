#include "../src/ast.h"
#include "../src/parser.h"
#include "framework.h"

#include "test_utils.h"

TEST(ast_free) {
    create_temp_file("test_ast.lt", "fn main(): s32 { return 0; };");
    read_file("test_ast.lt");

    struct program_ast* prog = parse_program();
    ASSERT_TRUE(prog != NULL);

    free_ast(prog);

    remove("test_ast.lt");
}
