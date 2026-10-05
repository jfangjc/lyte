#include "common.h"
#include "framework.h"
#include "lexer.h"
#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct expected_token {
    int type;
    const char* text;
    int line;
};

static void assert_tokens(const char* source, const struct expected_token* expected, size_t count) {
    create_temp_file("test_scanner.lt", source);
    char* content = read_file("test_scanner.lt");
    for (size_t i = 0; i < count; i++) {
        struct token* token = next_token();
        test_assert_true(token != NULL, "expected a token", __FILE__, __LINE__);
        test_assert_eq(expected[i].type, token->type, __FILE__, __LINE__);
        test_assert_eq((int)strlen(expected[i].text), token->length, __FILE__, __LINE__);
        test_assert_true(memcmp(token->start_pos, expected[i].text, (size_t)token->length) == 0, "token text matches",
                         __FILE__, __LINE__);
        test_assert_eq(expected[i].line, token->line_num, __FILE__, __LINE__);
        free(token);
    }
    test_assert_true(next_token() == NULL, "expected end of input", __FILE__, __LINE__);
    free(content);
    remove("test_scanner.lt");
}

#define ASSERT_TOKENS(source, expected) assert_tokens(source, expected, sizeof(expected) / sizeof(expected[0]))

void test_scanner_basic(void) {
    const struct expected_token expected[] = {
        {TOK_VAR, "var", 1}, {TOK_ID, "_value2", 1}, {TOK_ASSIGN, "=", 1}, {TOK_INT, "10", 1}, {';', ";", 1},
    };
    ASSERT_TOKENS("var _value2 = 10;", expected);
}

void test_scanner_keywords(void) {
    const struct expected_token expected[] = {
        {TOK_MODULE, "module", 1},
        {TOK_EXPORT, "export", 1},
        {TOK_IMPORT, "import", 1},
        {TOK_AS, "as", 1},
        {TOK_TRANSPARENT, "transparent", 1},
        {TOK_TYPE, "type", 2},
        {TOK_FN, "fn", 2},
        {TOK_REF, "ref", 2},
        {TOK_NEW, "new", 2},
        {TOK_LET, "let", 3},
        {TOK_VAR, "var", 3},
        {TOK_MUT, "mut", 3},
        {TOK_TAKE, "take", 3},
        {TOK_FROM, "from", 3},
        {TOK_IF, "if", 4},
        {TOK_ELSE, "else", 4},
        {TOK_FOR, "for", 4},
        {TOK_MATCH, "match", 4},
        {TOK_RETURN, "return", 4},
        {TOK_BREAK, "break", 4},
        {TOK_CONTINUE, "continue", 4},
        {TOK_UNSAFE, "unsafe", 5},
        {TOK_TRUE, "true", 6},
        {TOK_FALSE, "false", 6},
    };
    ASSERT_TOKENS("module export import as transparent\n"
                  "type fn ref new\n"
                  "let var mut take from\n"
                  "if else for match return break continue\n"
                  "unsafe\ntrue false",
                  expected);
}

void test_scanner_identifiers(void) {
    const struct expected_token expected[] = {
        {TOK_ID, "const", 1},  {TOK_ID, "safe", 1},   {TOK_ID, "out", 1},
        {TOK_ID, "Copy", 1},   {TOK_ID, "Match", 1},  {TOK_ID, "match_value", 1},
        {TOK_ID, "match2", 1}, {TOK_ID, "_match", 1}, {TOK_ID, "transparent_value", 1},
        {TOK_ID, "_", 1},      {TOK_ID, "std", 1},
        {TOK_ID, "ma", 1},     {TOK_ID, "matc", 1}, {TOK_ID, "matcha", 1},
        {TOK_ID, "asa", 1},    {TOK_ID, "vara", 1}, {TOK_ID, "aa", 1}, {TOK_ID, "zz", 1},
        {TOK_ID, "heap", 1}, {TOK_ID, "ref_value", 1}, {TOK_ID, "from_value", 1},
    };
    ASSERT_TOKENS("const safe out Copy Match match_value match2 _match transparent_value _ std "
                  "ma matc matcha asa vara aa zz heap ref_value from_value", expected);
}

void test_scanner_operators(void) {
    const struct expected_token expected[] = {
        {'+', "+", 1},
        {'-', "-", 1},
        {'*', "*", 1},
        {'/', "/", 1},
        {'%', "%", 1},
        {TOK_ASSIGN, "=", 1},
        {TOK_ADD_ASSIGN, "+=", 1},
        {TOK_SUB_ASSIGN, "-=", 1},
        {TOK_MUL_ASSIGN, "*=", 1},
        {TOK_DIV_ASSIGN, "/=", 1},
        {TOK_EQEQ, "==", 1},
        {TOK_NOTEQ, "!=", 1},
        {'>', ">", 1},
        {'<', "<", 1},
        {TOK_GTEQ, ">=", 1},
        {TOK_LTEQ, "<=", 1},
        {'!', "!", 1},
        {'&', "&", 1},
        {TOK_AND, "&&", 1},
        {'|', "|", 1},
        {TOK_OR, "||", 1},
        {TOK_FAT_ARROW, "=>", 1},
        {'.', ".", 1},
        {TOK_RANGE, "..", 1},
    };
    ASSERT_TOKENS("+ - * / % = += -= *= /= == != > < >= <= ! & && | || => . ..", expected);
}

void test_scanner_numbers_and_ranges(void) {
    const struct expected_token expected[] = {
        {TOK_INT, "0", 1},    {TOK_RANGE, "..", 1},  {TOK_INT, "2", 1},     {TOK_FLOAT, "1.5", 1},
        {TOK_RANGE, "..", 1}, {TOK_FLOAT, "2.5", 1}, {TOK_FLOAT, "3.0", 1}, {TOK_INT, "42", 1},
    };
    ASSERT_TOKENS("0..2 1.5..2.5 3.0 42", expected);
}

void test_scanner_comments(void) {
    const struct expected_token expected[] = {
        {TOK_LET, "let", 2}, {TOK_ID, "x", 2}, {TOK_ASSIGN, "=", 2},    {TOK_INT, "1", 2}, {';', ";", 2},
        {TOK_VAR, "var", 4}, {TOK_ID, "y", 4}, {TOK_ASSIGN, "=", 4},    {TOK_INT, "2", 4}, {';', ";", 4},
        {'/', "/", 5},       {'/', "/", 5},    {TOK_ID, "ordinary", 5},
    };
    ASSERT_TOKENS("# first line\nlet x = 1; # trailing comment\r\n\n\tvar y = 2;\n// ordinary\n# EOF", expected);
}

void test_scanner_strings(void) {
    const struct expected_token expected[] = {
        {TOK_STRING, "\"Hello # World\"", 1},
        {TOK_STRING, "\"\\n\\r\\t\\0\\\\\\\"\"", 1},
        {TOK_STRING, "\"caf\xC3\xA9 \xE4\xB8\xAD \xF0\x9F\x98\x80\"", 1},
        {TOK_STRING, "\"first\nsecond\"", 1},
        {TOK_LET, "let", 2},
    };
    ASSERT_TOKENS("\"Hello # World\" \"\\n\\r\\t\\0\\\\\\\"\" "
                  "\"caf\xC3\xA9 \xE4\xB8\xAD \xF0\x9F\x98\x80\" \"first\nsecond\" let",
                  expected);
}

void test_scanner_characters(void) {
    const struct expected_token expected[] = {
        {TOK_CHAR, "'a'", 1}, {TOK_CHAR, "' '", 1}, {TOK_CHAR, "'#'", 1},
        {TOK_CHAR, "'\"'", 1}, {TOK_CHAR, "'\\n'", 1}, {TOK_CHAR, "'\\r'", 1},
        {TOK_CHAR, "'\\t'", 1}, {TOK_CHAR, "'\\0'", 1}, {TOK_CHAR, "'\\\\'", 1},
        {TOK_CHAR, "'\\''", 1}, {TOK_CHAR, "'\\\"'", 1},
        {TOK_CHAR, "'\xC3\xA9'", 2}, {TOK_CHAR, "'\xE4\xB8\xAD'", 2},
        {TOK_CHAR, "'\xF0\x9F\x98\x80'", 2}, {TOK_LET, "let", 2},
    };
    ASSERT_TOKENS("'a' ' ' '#' '\"' '\\n' '\\r' '\\t' '\\0' '\\\\' '\\'' '\\\"'\n"
                  "'\xC3\xA9' '\xE4\xB8\xAD' '\xF0\x9F\x98\x80' let", expected);
}

void test_scanner_match_and_arrays(void) {
    const struct expected_token expected[] = {
        {TOK_MATCH, "match", 1},
        {TOK_ID, "state", 1},
        {'{', "{", 1},
        {TOK_ID, "Some", 1},
        {'(', "(", 1},
        {TOK_LET, "let", 1},
        {TOK_ID, "value", 1},
        {')', ")", 1},
        {TOK_FAT_ARROW, "=>", 1},
        {'{', "{", 1},
        {TOK_RETURN, "return", 1},
        {'&', "&", 1},
        {TOK_ID, "items", 1},
        {'[', "[", 1},
        {TOK_INT, "0", 1},
        {TOK_RANGE, "..", 1},
        {TOK_INT, "2", 1},
        {']', "]", 1},
        {';', ";", 1},
        {'}', "}", 1},
        {TOK_ID, "None", 1},
        {TOK_FAT_ARROW, "=>", 1},
        {'{', "{", 1},
        {TOK_RETURN, "return", 1},
        {'[', "[", 1},
        {TOK_INT, "0", 1},
        {';', ";", 1},
        {TOK_INT, "4", 1},
        {']', "]", 1},
        {';', ";", 1},
        {'}', "}", 1},
        {'}', "}", 1},
    };
    ASSERT_TOKENS("match state { Some(let value) => { return &items[0..2]; } None => { return [0; 4]; } }", expected);
}
