#include "framework.h"
#include <stdio.h>

// Test declarations
void test_ht_basic(void);
void test_ht_expansion(void);
void test_ht_collision(void);
void test_parser_fn_decl(void);
void test_parser_var_decl(void);
void test_parser_let_decl(void);
void test_parser_module_decl(void);
void test_parser_import_decl(void);
void test_parser_export_prefix_decl(void);
void test_parser_export_block_decl(void);
void test_scanner_basic(void);
void test_scanner_keywords(void);
void test_scanner_identifiers(void);
void test_scanner_numbers_and_ranges(void);
void test_scanner_match_and_arrays(void);
void test_scanner_operators(void);
void test_scanner_comments(void);
void test_scanner_strings(void);
void test_scanner_characters(void);
void test_parser_char_literal(void);
void test_pointers_parsing(void);

int main(void) {
    register_test("ht_basic", test_ht_basic);
    register_test("ht_expansion", test_ht_expansion);
    register_test("ht_collision", test_ht_collision);
    register_test("parser_fn_decl", test_parser_fn_decl);
    register_test("parser_var_decl", test_parser_var_decl);
    register_test("parser_let_decl", test_parser_let_decl);
    register_test("parser_module_decl", test_parser_module_decl);
    register_test("parser_import_decl", test_parser_import_decl);
    register_test("parser_export_prefix_decl", test_parser_export_prefix_decl);
    register_test("parser_export_block_decl", test_parser_export_block_decl);
    register_test("scanner_basic", test_scanner_basic);
    register_test("scanner_keywords", test_scanner_keywords);
    register_test("scanner_identifiers", test_scanner_identifiers);
    register_test("scanner_numbers_and_ranges", test_scanner_numbers_and_ranges);
    register_test("scanner_match_and_arrays", test_scanner_match_and_arrays);
    register_test("scanner_operators", test_scanner_operators);
    register_test("scanner_comments", test_scanner_comments);
    register_test("scanner_strings", test_scanner_strings);
    register_test("scanner_characters", test_scanner_characters);
    register_test("parser_char_literal", test_parser_char_literal);
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
