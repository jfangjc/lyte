#include "scanner.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static void produce_token(struct token* token, char* start_pos, int type);
static int token_cmp(struct token* token, char* target, int target_length);
static int generate_type(struct token* token);

static int line_num = 1;
static int col_num = 1;

static char* curr;

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

struct token* next() {
    struct token* token = malloc(sizeof(struct token));
    token->length = 0;

	while (*curr != '\0') {
        if ((*curr >= 'A' && *curr <= 'Z') || (*curr >= 'a' && *curr <= 'z') || *curr == '_') {
            produce_token(token, curr, TOK_ID);
            while ((*curr >= 'A' && *curr <= 'Z') || (*curr >= 'a' && *curr <= 'z') || *curr == '_'
            || (*curr >= '0' && *curr <= '9')) {
                curr++;
                token->length += 1;
            }
            token->type = generate_type(token);
            return token;
        }
        else if (*curr >= '0' && *curr <= '9') {
            produce_token(token, curr, TOK_NUM);
            while (*curr >= '0' && *curr <= '9') {
                curr++;
                token->length += 1;
            }
            return token;
        }
        else if (*curr == '=') {
            produce_token(token, curr, *curr);
            curr++;
            if (*curr == '=') {
                curr++;
                token->length += 1;
                token->type = TOK_EQUAL;
            }
            return token;
        }
        else if (*curr == '>') {
            produce_token(token, curr, *curr);
            curr++;
            if (*curr == '=') {
                curr++;
                token->length += 1;
                token->type = TOK_GREATEREQUAL;
            }
            return token;
        }
        else if (*curr == '<') {
            produce_token(token, curr, *curr);
            curr++;
            if (*curr == '=') {
                curr++;
                token->length += 1;
                token->type = TOK_LESSEQUAL;
            }
            return token;
        }
        else if (*curr == '!') {
            produce_token(token, curr, *curr);
            curr++;
            if (*curr == '=') {
                curr++;
                token->length += 1;
                token->type = TOK_NOTEQUAL;
            }
            return token;
        }
        else if (*curr == '#') {
            while (*(curr++) != '\n') { }
            continue;
        }
        else if (*curr == '\n') {
            line_num += 1;
            curr++;
            continue;
        }
        else if (*curr == ' ') {
            curr++;
            continue;
        }
        else {
            produce_token(token, curr, *curr);
            curr++;
            return token;
        }
	}
    return NULL;
}

static void produce_token(struct token* token, char* start_pos, int type) {
    token -> start_pos = start_pos;
    token -> type = type;
    token -> line_num = line_num;
}

static int token_cmp(struct token* token, char* target, int target_length) {
    if (token->length != target_length) {
        return 0;
    }
    return (memcmp(token->start_pos, target, token->length) == 0);
}

static int generate_type(struct token* token) {
    if (token_cmp(token, "si8", 3)) { return TOK_SI8; }
    else if (token_cmp(token, "si16", 4)) { return TOK_SI16; }
    else if (token_cmp(token, "si32", 4)) { return TOK_SI32; }
    else if (token_cmp(token, "si64", 4)) { return TOK_SI64; }
    else if (token_cmp(token, "ui8", 3)) { return TOK_UI8; }
    else if (token_cmp(token, "ui16", 4)) { return TOK_UI16; }
    else if (token_cmp(token, "ui32", 4)) { return TOK_UI32; }
    else if (token_cmp(token, "ui64", 4)) { return TOK_UI64; }
    else if (token_cmp(token, "fn", 2)) { return TOK_FUNCTION; }
    else if (token_cmp(token, "def", 3)) { return TOK_CONST; }
    else if (token_cmp(token, "var", 3)) { return TOK_VAR; }
    else if (token_cmp(token, "return", 6)) { return TOK_RETURN; }

	return TOK_IDENTIFIER;
}