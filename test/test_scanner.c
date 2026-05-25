#include "common.h"
#include "lexer.h"
#include "framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_utils.h"

void test_scanner_basic(void) {
    create_temp_file("test_basic.lt", "var x = 10");
    read_file("test_basic.lt");

    struct token* t = next_token();
    test_assert_eq(TOK_VAR, t->type, __FILE__, __LINE__);

    t = next_token();
    test_assert_eq(TOK_ID, t->type, __FILE__, __LINE__);
    test_assert_true(strncmp(t->start_pos, "x", (size_t)t->length) == 0,
                     "strncmp(t->start_pos, \"x\", (size_t)t->length) == 0",
                     __FILE__, __LINE__);

    t = next_token();
    test_assert_eq(TOK_ASSIGN, t->type, __FILE__, __LINE__);

    t = next_token();
    test_assert_eq(TOK_INT, t->type, __FILE__, __LINE__);
    test_assert_true(strncmp(t->start_pos, "10", (size_t)t->length) == 0,
                     "strncmp(t->start_pos, \"10\", (size_t)t->length) == 0",
                     __FILE__, __LINE__);

    remove("test_basic.lt");
}

void test_scanner_keywords(void) {
    create_temp_file("test_keywords.lt",
        "module export import type fn const var return if else for "
        "continue break unsafe");
    read_file("test_keywords.lt");

    test_assert_eq(TOK_MODULE, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_EXPORT, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_IMPORT, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_TYPE, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_FN, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_CONST, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_VAR, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_RETURN, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_IF, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ELSE, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_FOR, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_CONTINUE, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_BREAK, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_UNSAFE, next_token()->type, __FILE__, __LINE__);

    remove("test_keywords.lt");
}

void test_scanner_operators(void) {
    create_temp_file("test_ops.lt", "+ - * / == !=");
    read_file("test_ops.lt");

    test_assert_eq('+', next_token()->type, __FILE__, __LINE__);
    test_assert_eq('-', next_token()->type, __FILE__, __LINE__);
    test_assert_eq('*', next_token()->type, __FILE__, __LINE__);
    test_assert_eq('/', next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_EQEQ, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_NOTEQ, next_token()->type, __FILE__, __LINE__);

    remove("test_ops.lt");
}

void test_scanner_comments(void) {
    create_temp_file("test_comments.lt",
                     "var x = 1; # This is a comment\nvar y = 2;");
    read_file("test_comments.lt");

    test_assert_eq(TOK_VAR, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ID, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ASSIGN, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_INT, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(';', next_token()->type, __FILE__, __LINE__);

    // Comment should be skipped
    test_assert_eq(TOK_VAR, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ID, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ASSIGN, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_INT, next_token()->type, __FILE__, __LINE__);

    remove("test_comments.lt");
}

void test_scanner_strings(void) {
    create_temp_file("test_strings.lt", "var s = \"Hello World\";");
    read_file("test_strings.lt");

    test_assert_eq(TOK_VAR, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ID, next_token()->type, __FILE__, __LINE__);
    test_assert_eq(TOK_ASSIGN, next_token()->type, __FILE__, __LINE__);

    struct token* t = next_token();
    test_assert_eq(TOK_STRING, t->type, __FILE__, __LINE__);

    remove("test_strings.lt");
}
