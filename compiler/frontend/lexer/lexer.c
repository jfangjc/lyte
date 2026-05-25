#include "lexer.h"
#include "keywords.h"
#include "common.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct lexer g_lexer;

static int is_alpha(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int is_num(int c) {
    return (c >= '0' && c <= '9');
}

static void advance(struct lexer* lexer, struct token* token) {
    lexer->col_num += 1;
    lexer->curr += 1;
    token->length += 1;
}

static void produce_token(struct lexer* lexer, struct token* token,
                          char* start_pos, int type) {
    token->start_pos = start_pos;
    token->type = type;
    token->line_num = lexer->line_num;
}

static struct token* lex_identifier(struct lexer* lexer, struct token* token) {
    produce_token(lexer, token, lexer->curr, TOK_ID);
    while (is_alpha(*lexer->curr) || is_num(*lexer->curr) || *lexer->curr == '_') {
        advance(lexer, token);
    }
    token->type = keyword_lookup(token->start_pos, token->length);
    return token;
}

static struct token* lex_number(struct lexer* lexer, struct token* token) {
    produce_token(lexer, token, lexer->curr, TOK_INT);
    int decimal_count = 0;

    while (is_num(*lexer->curr) || *lexer->curr == '.') {
        if (*lexer->curr == '.') {
            decimal_count += 1;
            token->type = TOK_FLOAT;
            if (decimal_count > 1) {
                error("Invalid floating point number");
            }
        }
        advance(lexer, token);
    }
    return token;
}

// Scan a double quote string
static struct token* lex_string(struct lexer* lexer, struct token* token) {
    produce_token(lexer, token, lexer->curr, TOK_STRING);
    advance(lexer, token); // opening quote
    while (*lexer->curr != '\"') {
        advance(lexer, token);
    }
    advance(lexer, token); // closing quote
    return token;
}

// Scan a single quote character
static struct token* lex_char(struct lexer* lexer, struct token* token) {
    produce_token(lexer, token, lexer->curr, TOK_CHAR);
    advance(lexer, token); // opening quote
    while (*lexer->curr != '\'') {
        advance(lexer, token);
    }
    advance(lexer, token); // closing quote
    return token;
}

// Scan a one or two-character operator
static struct token* lex_operator(struct lexer* lexer, struct token* token,
                                  int one_type, char two_char, int two_type) {
    produce_token(lexer, token, lexer->curr, one_type);
    advance(lexer, token);
    if (two_char != '\0' && *lexer->curr == two_char) {
        advance(lexer, token);
        token->type = two_type;
    }
    return token;
}

// Skip comments
static void skip_line_comment(struct lexer* lexer) {
    while (*lexer->curr != '\n' && *lexer->curr != '\0') {
        lexer->curr++;
    }
}

// Skip whitespace
static void skip_whitespace(struct lexer* lexer) {
    lexer->curr += 1;
}

//  updating line count for newline
static void skip_newline(struct lexer* lexer) {
    lexer->line_num += 1;
    lexer->curr += 1;
}

void lexer_init(struct lexer* lexer, char* source) {
    lexer->curr = source;
    lexer->line_num = 1;
    lexer->col_num = 1;
}

char* read_file(char* path) {
    FILE* file = NULL;
#ifdef _MSC_VER
    fopen_s(&file, path, "r");
#else
    file = fopen(path, "r");
#endif

    if (file) {
        fseek(file, 0, SEEK_END);
        size_t size = (size_t)ftell(file);
        fseek(file, 0, SEEK_SET);

        if (size) {
            char* content = (char*)malloc(size + 1);
            fread(content, size, 1, file);
            fclose(file);
            content[size] = '\0';
            lexer_init(&g_lexer, content);
            return content;
        }
        error("File does not exist.");
        return NULL;
    }
    error("File does not exist.");
    return NULL;
}

struct token* next_token(void) {
    struct lexer* lexer = &g_lexer;

    struct token* token = malloc(sizeof(struct token));
    token->length = 0;

    while (*lexer->curr != '\0') {
        // Identifiers and keywords
        if (is_alpha(*lexer->curr)) {
            return lex_identifier(lexer, token);
        }
        // Numeric value
        if (is_num(*lexer->curr)) {
            return lex_number(lexer, token);
        }

        // Operators
        switch (*lexer->curr) {
        case '=': {
            struct token* result = lex_operator(lexer, token, '=', '=', TOK_EQEQ);
            if (result->type == '=') {
                result->type = TOK_ASSIGN;
            }
            return result;
        }
        case '>':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_GTEQ);
        case '<':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_LTEQ);
        case '!':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_NOTEQ);
        case '+':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_ADD_ASSIGN);
        case '-':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_SUB_ASSIGN);
        case '*':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_MUL_ASSIGN);
        case '/':
            return lex_operator(lexer, token, *lexer->curr, '=', TOK_DIV_ASSIGN);
        case '&':
            return lex_operator(lexer, token, *lexer->curr, '&', TOK_AND);

        // String and char
        case '\"':
            return lex_string(lexer, token);
        case '\'':
            return lex_char(lexer, token);

        // Comments
        case '#':
            skip_line_comment(lexer);
            continue;

        // Whitespace
        case '\n':
            skip_newline(lexer);
            continue;
        case ' ':
        case '\t':
        case '\r':
        case '\v':
        case '\f':
            skip_whitespace(lexer);
            continue;

        // Symbols
        default:
            produce_token(lexer, token, lexer->curr, *lexer->curr);
            advance(lexer, token);
            return token;
        }
    }

    free(token);
    return NULL;
}
