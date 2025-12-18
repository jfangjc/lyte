#include "../src/compiler/common.h"
#include "../src/compiler/scanner.h"
#include "framework.h"

#include "test_utils.h"

TEST(scanner_basic) {
    create_temp_file("test_basic.lt", "let x = 10");
    read_file("test_basic.lt");

    struct token* t = next_token();
    ASSERT_EQ(TOK_LET, t->type);

    t = next_token();
    ASSERT_EQ(TOK_ID, t->type);
    ASSERT_TRUE(strncmp(t->start_pos, "x", (size_t)t->length) == 0);

    t = next_token();
    ASSERT_EQ(TOK_ASSIGN, t->type);

    t = next_token();
    ASSERT_EQ(TOK_INT, t->type);
    ASSERT_TRUE(strncmp(t->start_pos, "10", (size_t)t->length) == 0);

    remove("test_basic.lt");
}

TEST(scanner_keywords) {
    create_temp_file("test_keywords.lt", "fn if else return");
    read_file("test_keywords.lt");

    ASSERT_EQ(TOK_FN, next_token()->type);
    ASSERT_EQ(TOK_IF, next_token()->type);
    ASSERT_EQ(TOK_ELSE, next_token()->type);
    ASSERT_EQ(TOK_RETURN, next_token()->type);

    remove("test_keywords.lt");
}

TEST(scanner_operators) {
    create_temp_file("test_ops.lt", "+ - * / == !=");
    read_file("test_ops.lt");

    ASSERT_EQ('+', next_token()->type);
    ASSERT_EQ('-', next_token()->type);
    ASSERT_EQ('*', next_token()->type);
    ASSERT_EQ('/', next_token()->type);
    ASSERT_EQ(TOK_EQEQ, next_token()->type);
    ASSERT_EQ(TOK_NOTEQ, next_token()->type);

    remove("test_ops.lt");
}

TEST(scanner_comments) {
    create_temp_file("test_comments.lt",
                     "let x = 1; // This is a comment\nlet y = 2;");
    read_file("test_comments.lt");

    ASSERT_EQ(TOK_LET, next_token()->type);
    ASSERT_EQ(TOK_ID, next_token()->type);
    ASSERT_EQ(TOK_ASSIGN, next_token()->type);
    ASSERT_EQ(TOK_INT, next_token()->type);
    ASSERT_EQ(';', next_token()->type);

    // Comment should be skipped
    ASSERT_EQ(TOK_LET, next_token()->type);
    ASSERT_EQ(TOK_ID, next_token()->type);
    ASSERT_EQ(TOK_ASSIGN, next_token()->type);
    ASSERT_EQ(TOK_INT, next_token()->type);

    remove("test_comments.lt");
}

TEST(scanner_strings) {
    create_temp_file("test_strings.lt", "let s = \"Hello World\";");
    read_file("test_strings.lt");

    ASSERT_EQ(TOK_LET, next_token()->type);
    ASSERT_EQ(TOK_ID, next_token()->type);
    ASSERT_EQ(TOK_ASSIGN, next_token()->type);

    struct token* t = next_token();
    ASSERT_EQ(TOK_STRING, t->type);

    remove("test_strings.lt");
}
