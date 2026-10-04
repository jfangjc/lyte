#include "lexer.h"
#include "common.h"
#include "error.h"
#include "file.h"
#include "keywords.h"

#include <stdlib.h>

static struct lexer g_lexer;

static int is_alpha(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

static int is_num(int c) { return (c >= '0' && c <= '9'); }

static void advance(struct lexer* lexer, struct token* token) {
    if (*lexer->curr == '\n') {
        lexer->line_num += 1;
        lexer->col_num = 1;
    }
    else {
        lexer->col_num += 1;
    }
    lexer->curr += 1;
    token->length += 1;
}

static void produce_token(struct lexer* lexer, struct token* token, char* start_pos, int type) {
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
    while (is_num(*lexer->curr)) {
        advance(lexer, token);
    }
    // Leave both dots of a range for lex_operator
    if (*lexer->curr == '.' && lexer->curr[1] != '.') {
        token->type = TOK_FLOAT;
        advance(lexer, token);
        while (is_num(*lexer->curr)) {
            advance(lexer, token);
        }
    }
    return token;
}

// Validate and consume one UTF-8 scalar value inside a quoted literal.
static void lex_utf8_char(struct lexer* lexer, struct token* token, const char* error_message) {
    unsigned char lead = (unsigned char)*lexer->curr;
    int length;
    unsigned int codepoint;
    unsigned int minimum;
    if (lead >= 0xC2 && lead <= 0xDF) {
        length = 2;
        codepoint = lead & 0x1Fu;
        minimum = 0x80u;
    }
    else if (lead >= 0xE0 && lead <= 0xEF) {
        length = 3;
        codepoint = lead & 0x0Fu;
        minimum = 0x800u;
    }
    else if (lead >= 0xF0 && lead <= 0xF4) {
        length = 4;
        codepoint = lead & 0x07u;
        minimum = 0x10000u;
    }
    else {
        error(error_message);
        return;
    }
    advance(lexer, token);
    for (int i = 1; i < length; i++) {
        unsigned char byte = (unsigned char)*lexer->curr;
        if (byte < 0x80 || byte > 0xBF) {
            error(error_message);
        }
        codepoint = (codepoint << 6) | (byte & 0x3Fu);
        advance(lexer, token);
    }
    if (codepoint < minimum || codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu)) {
        error(error_message);
    }
}

static struct token* lex_string(struct lexer* lexer, struct token* token) {
    produce_token(lexer, token, lexer->curr, TOK_STRING);
    advance(lexer, token); // opening quote
    while (*lexer->curr != '\"') {
        if (*lexer->curr == '\0') {
            error("Unterminated string literal");
        }
        if (*lexer->curr == '\\') {
            advance(lexer, token);
            switch (*lexer->curr) {
            case 'n':
            case 'r':
            case 't':
            case '0':
            case '\\':
            case '"':
                advance(lexer, token);
                break;
            case '\0':
                error("Unterminated string literal");
                break;
            default:
                error("Invalid string escape");
            }
        }
        else if ((unsigned char)*lexer->curr >= 0x80) {
            lex_utf8_char(lexer, token, "Invalid UTF-8 in string literal");
        }
        else {
            advance(lexer, token);
        }
    }
    advance(lexer, token); // closing quote
    return token;
}

static struct token* lex_char_literal(struct lexer* lexer, struct token* token) {
    produce_token(lexer, token, lexer->curr, TOK_CHAR);
    advance(lexer, token); // opening quote
    if (*lexer->curr == '\0' || *lexer->curr == '\n' || *lexer->curr == '\r') {
        error("Unterminated character literal");
    }
    if (*lexer->curr == '\'') {
        error("Character literal must contain exactly one character");
    }

    if (*lexer->curr == '\\') {
        advance(lexer, token);
        switch (*lexer->curr) {
        case 'n':
        case 'r':
        case 't':
        case '0':
        case '\\':
        case '\'':
        case '"':
            advance(lexer, token);
            break;
        case '\0':
        case '\n':
        case '\r':
            error("Unterminated character literal");
            break;
        default:
            error("Invalid character escape");
        }
    }
    else if ((unsigned char)*lexer->curr >= 0x80) {
        lex_utf8_char(lexer, token, "Invalid UTF-8 in character literal");
    }
    else {
        advance(lexer, token);
    }

    if (*lexer->curr == '\0' || *lexer->curr == '\n' || *lexer->curr == '\r') {
        error("Unterminated character literal");
    }
    if (*lexer->curr != '\'') {
        error("Character literal must contain exactly one character");
    }
    advance(lexer, token); // closing quote
    return token;
}

// Scan a one or two-character operator
static struct token* lex_operator(struct lexer* lexer, struct token* token, int one_type, char two_char, int two_type) {
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
        lexer->col_num++;
    }
}

// Skip whitespace
static void skip_whitespace(struct lexer* lexer) {
    lexer->curr += 1;
    lexer->col_num += 1;
}

//  updating line count for newline
static void skip_newline(struct lexer* lexer) {
    lexer->line_num += 1;
    lexer->col_num = 1;
    lexer->curr += 1;
}

void lexer_init(struct lexer* lexer, char* source) {
    lexer->curr = source;
    lexer->line_num = 1;
    lexer->col_num = 1;
}

char* read_file(char* path) {
    char* content = read_file_contents(path);
    if (content[0] == '\0') {
        free(content);
        error("File does not exist.");
    }

    lexer_init(&g_lexer, content);
    return content;
}

struct token* next_token(void) {
    struct lexer* lexer = &g_lexer;
    struct token* token = malloc(sizeof(struct token));
    if (token == NULL) {
        error("Out of memory");
    }
    token->length = 0;

    while (*lexer->curr != '\0') {
        // Identifiers and keywords
        if (is_alpha(*lexer->curr) || *lexer->curr == '_') {
            return lex_identifier(lexer, token);
        }
        // Numeric value
        if (is_num(*lexer->curr)) {
            return lex_number(lexer, token);
        }

        // Operators
        switch (*lexer->curr) {
        case '=': {
            if (lexer->curr[1] == '>') {
                return lex_operator(lexer, token, TOK_ASSIGN, '>', TOK_FAT_ARROW);
            }
            return lex_operator(lexer, token, TOK_ASSIGN, '=', TOK_EQEQ);
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
        case '|':
            return lex_operator(lexer, token, '|', '|', TOK_OR);
        case '.':
            return lex_operator(lexer, token, '.', '.', TOK_RANGE);

        // Quoted literals
        case '\"':
            return lex_string(lexer, token);
        case '\'':
            return lex_char_literal(lexer, token);

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
        case '{':
        case '}':
        case '(':
        case ')':
        case '[':
        case ']':
        case ',':
        case ':':
        case ';':
        case '%':
            produce_token(lexer, token, lexer->curr, *lexer->curr);
            advance(lexer, token);
            return token;
        default:
            error("Invalid source character");
        }
    }

    free(token);
    return NULL;
}
