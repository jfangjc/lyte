#include "common.h"
#include "codegen.h"
#include "framework.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_utils.h"

void test_parser_char_literal(void) {
    create_temp_file("test_char.lt", "fn character(): char { return '\xF0\x9F\x98\x80'; }\n"
                                     "fn escaped(): char { return '\\''; }\n"
                                     "fn newline(): char { return '\\n'; }\n"
                                     "fn nul(): char { return '\\0'; }\n"
                                     "fn two_bytes(): char { return '\xC3\xA9'; }\n"
                                     "fn three_bytes(): char { return '\xE4\xB8\xAD'; }\n"
                                     "fn ascii(): char { let c: char = 'a'; return c; }");
    char* content = read_file("test_char.lt");
    struct program_ast* prog = parse_program();
    struct expr* value = prog->fn_decls->body->stmt.return_stmt->expr;
    test_assert_eq(EXPR_VALUE, (int)value->type, __FILE__, __LINE__);
    test_assert_eq(TOK_CHAR, value->exprs.value_expr->type, __FILE__, __LINE__);
    gen("test_char.ll", prog);
    FILE* output = fopen("test_char.ll", "r");
    test_assert_true(output != NULL, "generated character IR exists", __FILE__, __LINE__);
    char ir[4096];
    size_t size = fread(ir, 1, sizeof(ir) - 1, output);
    ir[size] = '\0';
    fclose(output);
    const char* expected[] = {"ret i32 128512", "ret i32 39", "ret i32 10", "ret i32 0",
                              "ret i32 233", "ret i32 20013", "store i32 97"};
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        test_assert_true(strstr(ir, expected[i]) != NULL, expected[i], __FILE__, __LINE__);
    }
    free(content);
    remove("test_char.lt");
    remove("test_char.ll");
}

void test_parser_fn_decl(void) {
    create_temp_file("test_fn.lt", "fn main(): s32 { return 0; }");
    read_file("test_fn.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);
    test_assert_true(prog->fn_decls != NULL, "prog->fn_decls != NULL", __FILE__, __LINE__);

    struct fn_decl* fn = prog->fn_decls;
    test_assert_true(fn->name != NULL, "fn->name != NULL", __FILE__, __LINE__);
    test_assert_true(strncmp(fn->name->start_pos, "main", 4) == 0, "strncmp(fn->name->start_pos, \"main\", 4) == 0",
                     __FILE__, __LINE__);
    test_assert_true(fn->type != NULL, "fn->type != NULL", __FILE__, __LINE__);
    test_assert_eq(0, fn->type->kind, __FILE__, __LINE__);
    test_assert_true(strncmp(fn->type->name->start_pos, "s32", 3) == 0,
                     "strncmp(fn->type->name->start_pos, \"s32\", 3) == 0", __FILE__, __LINE__);

    remove("test_fn.lt");
}

void test_parser_var_decl(void) {
    create_temp_file("test_var.lt", "fn test(): s32 { var x: s32 = 10; }");
    read_file("test_var.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);
    test_assert_true(prog->fn_decls != NULL, "prog->fn_decls != NULL", __FILE__, __LINE__);

    struct fn_decl* fn = prog->fn_decls;
    struct stmt* body = fn->body;
    test_assert_true(body != NULL, "body != NULL", __FILE__, __LINE__);

    test_assert_true(body->stmt.var_decl != NULL, "body->stmt.var_decl != NULL", __FILE__, __LINE__);
    struct var_decl* var = body->stmt.var_decl;
    test_assert_true(strncmp(var->name->start_pos, "x", 1) == 0, "strncmp(var->name->start_pos, \"x\", 1) == 0",
                     __FILE__, __LINE__);
    test_assert_true(var->type != NULL, "var->type != NULL", __FILE__, __LINE__);
    test_assert_eq(0, var->type->kind, __FILE__, __LINE__);
    test_assert_true(strncmp(var->type->name->start_pos, "s32", 3) == 0,
                     "strncmp(var->type->name->start_pos, \"s32\", 3) == 0", __FILE__, __LINE__);

    remove("test_var.lt");
}

void test_parser_let_decl(void) {
    create_temp_file("test_let.lt", "let limit: s32 = 10; fn main(): s32 { let value = limit; return value; }");
    read_file("test_let.lt");
    struct program_ast* prog = parse_program();
    test_assert_true(prog->var_decls != NULL, "module let declaration exists", __FILE__, __LINE__);
    test_assert_eq(1, prog->var_decls->is_const, __FILE__, __LINE__);
    struct var_decl* local = prog->fn_decls->body->stmt.var_decl;
    test_assert_true(local != NULL, "local let declaration exists", __FILE__, __LINE__);
    test_assert_eq(1, local->is_const, __FILE__, __LINE__);
    remove("test_let.lt");
}

void test_parser_module_decl(void) {
    create_temp_file("test_module.lt", "module app.main\nfn main(): s32 { var x: s32 = 42; }");
    read_file("test_module.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog != NULL, "prog != NULL", __FILE__, __LINE__);
    test_assert_true(prog->module_name != NULL, "prog->module_name != NULL", __FILE__, __LINE__);
    test_assert_true(strncmp(prog->module_name->start_pos, "app", 3) == 0,
                     "strncmp(prog->module_name->start_pos, \"app\", 3) == 0", __FILE__, __LINE__);
    test_assert_str_eq("app.main", prog->module_path, __FILE__, __LINE__);
    test_assert_true(prog->fn_decls != NULL, "prog->fn_decls != NULL", __FILE__, __LINE__);

    remove("test_module.lt");
}

void test_parser_import_decl(void) {
    create_temp_file("test_import.lt", "module app.main\n"
                                       "import platform.io as io\n"
                                       "fn main(): s32 { io.print(\"starting\"); return 0; }");
    read_file("test_import.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog->imports != NULL, "prog->imports != NULL", __FILE__, __LINE__);
    test_assert_str_eq("platform.io", prog->imports->module_path, __FILE__, __LINE__);
    test_assert_true(prog->imports->alias != NULL, "prog->imports->alias != NULL", __FILE__, __LINE__);
    test_assert_true(strncmp(prog->imports->alias->start_pos, "io", 2) == 0,
                     "strncmp(prog->imports->alias->start_pos, \"io\", 2) == 0", __FILE__, __LINE__);

    struct expr* call_expr = prog->fn_decls->body->stmt.expr_stmt->expr;
    test_assert_eq(EXPR_CALL, (int)call_expr->type, __FILE__, __LINE__);
    test_assert_str_eq("io.print", call_expr->exprs.call_expr->name, __FILE__, __LINE__);

    remove("test_import.lt");
}

void test_parser_export_prefix_decl(void) {
    create_temp_file("test_export_prefix.lt", "module geometry.circle\n"
                                              "export type Circle = {\n"
                                              "    x: f32,\n"
                                              "}\n"
                                              "export fn create(): s32 { return 0; }\n"
                                              "export unsafe fn raw(): s32 { return 0; }");
    read_file("test_export_prefix.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog->type_decls != NULL, "prog->type_decls != NULL", __FILE__, __LINE__);
    test_assert_eq(1, prog->type_decls->is_exported, __FILE__, __LINE__);

    test_assert_true(prog->fn_decls != NULL, "prog->fn_decls != NULL", __FILE__, __LINE__);
    test_assert_eq(1, prog->fn_decls->is_exported, __FILE__, __LINE__);
    test_assert_eq(0, prog->fn_decls->is_unsafe, __FILE__, __LINE__);
    test_assert_true(prog->fn_decls->next != NULL, "prog->fn_decls->next != NULL", __FILE__, __LINE__);
    test_assert_eq(1, prog->fn_decls->next->is_exported, __FILE__, __LINE__);
    test_assert_eq(1, prog->fn_decls->next->is_unsafe, __FILE__, __LINE__);

    remove("test_export_prefix.lt");
}

void test_parser_export_block_decl(void) {
    create_temp_file("test_export_block.lt", "module app.contract\n"
                                             "export { one, Count }\n"
                                             "type Count = s32\n"
                                             "fn one(): s32 { return 1; }");
    read_file("test_export_block.lt");

    struct program_ast* prog = parse_program();
    test_assert_true(prog->exports != NULL, "prog->exports != NULL", __FILE__, __LINE__);
    test_assert_true(strncmp(prog->exports->name->start_pos, "one", 3) == 0,
                     "strncmp(prog->exports->name->start_pos, \"one\", 3) == 0", __FILE__, __LINE__);
    test_assert_true(prog->exports->next != NULL, "prog->exports->next != NULL", __FILE__, __LINE__);
    test_assert_true(strncmp(prog->exports->next->name->start_pos, "Count", 5) == 0,
                     "strncmp(prog->exports->next->name->start_pos, \"Count\", 5) == 0", __FILE__, __LINE__);

    remove("test_export_block.lt");
}
