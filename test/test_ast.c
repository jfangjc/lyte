#include "ast.h"
#include "parser.h"
#include "framework.h"
#include <stdio.h>
#include <stdlib.h>

#include "test_utils.h"

void test_ast_free(void) {
    create_temp_file("test_ast.lt", "fn main(): s32 { return 0; }");
    read_file("test_ast.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);

    free_ast(prog);

    remove("test_ast.lt");
}
