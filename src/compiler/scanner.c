#include "scanner.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static void produce_token(struct token* token, char* start_pos, int type);
static int token_cmp(struct token* token, char* target, int target_length);
static int generate_type(struct token* token);

static int is_alpha(int character);
static int is_num(int character);

static int line_num = 1;
static int col_num = 1;

static char* curr;

struct token* token;

static void advance() {
    col_num += 1;
    curr += 1;
    token->length += 1;
}

char* read_file(char* path) {
    FILE* File = fopen(path, "r");

    if (File) {
        fseek(File, 0, SEEK_END);
        size_t Size = ftell(File);
        fseek(File, 0, SEEK_SET);

        if (Size) {
			char* Content = (char*)malloc(Size + 1);
            fread(Content, Size, 1, File);
            fclose(File);
            Content[Size] = '\0';
            curr = Content;
            return Content;
        }
        error("File does not exist.");
        return NULL;
    }
    error("File does not exist.");
    return NULL;
}

struct token* next_token() {
    token = malloc(sizeof(struct token));
    token->length = 0;

	while (*curr != '\0') {
        if (is_alpha(*curr)) {
            produce_token(token, curr, TOK_ID);
            while (is_alpha(*curr) || is_num(*curr) || *curr == '_') {
                advance();
            }
            token->type = generate_type(token);
            return token;
        }
        else if (is_num(*curr)) {
            produce_token(token, curr, TOK_INT);
            int dec = 0;
            while (is_num(*curr) || *curr == '.') {
                if (*curr == '.') {
                    dec += 1;
                    token->type = TOK_FLOAT;
                    if (dec > 1) {
                        error("Invalid floating point number");
                    }
                }
                advance();
            }
            return token;
        }
        else if (*curr == '=') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_EQEQ;
            }
            return token;
        }
        else if (*curr == '>') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_GTEQ;
            }
            return token;
        }
        else if (*curr == '<') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_LTEQ;
            }
            return token;
        }
        else if (*curr == '!') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_NOTEQ;
            }
            return token;
        }
        else if (*curr == ':') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_ASSIGN;
            }
            return token;
        }
        else if (*curr == '\"') {
            produce_token(token, curr, TOK_STRING);
            advance();
            while (*curr != '\"') {
                advance();
            }
            advance();
            return token;
        }
        else if (*curr == '\'') {
            produce_token(token, curr, TOK_CHAR);
            advance();
            while (*curr != '\'') {
                advance();
            }
            advance();
            return token;
        }
        else if (*curr == '#') {
            while (*(curr++) != '\n') { }
            line_num += 1;
        }
        else if (*curr == '\n') {
            line_num += 1;
            curr += 1;
        }
        else if (*curr == ' ') {
            curr += 1;
        }
        else {
            produce_token(token, curr, *curr);
            advance();
            return token;
        }
	}
    return NULL;
}

static void produce_token(struct token* token, char* start_pos, int type) {
    token->start_pos = start_pos;
    token->type = type;
    token->line_num = line_num;
}

static int token_cmp(struct token* token, char* target, int target_length) {
    if (token->length != target_length) {
        return 0;
    }
    return (memcmp(token->start_pos, target, token->length) == 0);
}

static int generate_type(struct token* token) {
    if (token_cmp(token, "s8", 2)) { return TOK_S8; }
    else if (token_cmp(token, "s16", 3)) { return TOK_S16; }
    else if (token_cmp(token, "s32", 3)) { return TOK_S32; }
    else if (token_cmp(token, "s64", 3)) { return TOK_S64; }
    else if (token_cmp(token, "s128", 4)) { return TOK_S128; }

    else if (token_cmp(token, "u8", 2)) { return TOK_U8; }
    else if (token_cmp(token, "u16", 3)) { return TOK_U16; }
    else if (token_cmp(token, "u32", 3)) { return TOK_U32; }
    else if (token_cmp(token, "u64", 3)) { return TOK_U64; }
    else if (token_cmp(token, "u128", 4)) { return TOK_U128; }

    else if (token_cmp(token, "f32", 3)) { return TOK_F32; }
    else if (token_cmp(token, "f64", 3)) { return TOK_F64; }
    else if (token_cmp(token, "f128", 4)) { return TOK_F128; }

    else if (token_cmp(token, "let", 3)) { return TOK_LET; }
    else if (token_cmp(token, "fn", 2)) { return TOK_FN; }
    else if (token_cmp(token, "var", 3)) { return TOK_VAR; }
    else if (token_cmp(token, "addr", 4)) { return TOK_ADDR; }

    else if (token_cmp(token, "if", 2)) { return TOK_IF; }
    else if (token_cmp(token, "else", 4)) { return TOK_ELSE; }

    else if (token_cmp(token, "for", 3)) { return TOK_FOR; }
    else if (token_cmp(token, "break", 5)) { return TOK_BREAK; }
    else if (token_cmp(token, "continue", 8)) { return TOK_CONTINUE; }

    else if (token_cmp(token, "return", 6)) { return TOK_RETURN; }

	return TOK_ID;
}

static int is_alpha(int character) {
    return (character >= 'A' && character <= 'Z')
        || (character >= 'a' && character <= 'z');
}

static int is_num(int character) {
    return (character >= '0' && character <= '9');
}
