#include "framework.h"
#include <stdio.h>

// Test declarations
void test_ast_free(void);
void test_ht_basic(void);
void test_ht_expansion(void);
void test_ht_collision(void);
void test_parser_fn_decl(void);
void test_parser_var_decl(void);
void test_parser_entry_decl(void);
void test_scanner_basic(void);
void test_scanner_keywords(void);
void test_scanner_operators(void);
void test_scanner_comments(void);
void test_scanner_strings(void);
void test_pointers_parsing(void);

int main(void) {
    register_test("ast_free", test_ast_free);
    register_test("ht_basic", test_ht_basic);
    register_test("ht_expansion", test_ht_expansion);
    register_test("ht_collision", test_ht_collision);
    register_test("parser_fn_decl", test_parser_fn_decl);
    register_test("parser_var_decl", test_parser_var_decl);
    register_test("parser_entry_decl", test_parser_entry_decl);
    register_test("scanner_basic", test_scanner_basic);
    register_test("scanner_keywords", test_scanner_keywords);
    register_test("scanner_operators", test_scanner_operators);
    register_test("scanner_comments", test_scanner_comments);
    register_test("scanner_strings", test_scanner_strings);
    register_test("pointers_parsing", test_pointers_parsing);

    printf("Running %d tests...\n", test_count);
    int passed = 0;
    for (int i = 0; i < test_count; i++) {
        printf("[RUNNING] %s\n", tests[i].name);
        tests[i].func();
        printf("[PASSED]  %s\n", tests[i].name);
        passed++;
    }
    printf("\n%d/%d tests passed.\n", passed, test_count);
    return 0;
}
