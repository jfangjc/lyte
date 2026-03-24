#include "lexer.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static void produce_token(struct token* tok, char* start_pos, int type);
static int token_cmp(struct token* tok, char* target, int target_length);
static int generate_type(struct token* tok);

static int is_alpha(int character);
static int is_num(int character);

static int line_num = 1;
static int col_num = 1;

static char* curr;

struct token* token;

static void advance(void) {
    col_num += 1;
    curr += 1;
    token->length += 1;
}

char* read_file(char* path) {
    FILE* File = fopen(path, "r");

    if (File) {
        fseek(File, 0, SEEK_END);
        size_t Size = (size_t)ftell(File);
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

struct token* next_token(void) {
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
            else {
                token->type = TOK_ASSIGN;
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
            return token;
        }
        else if (*curr == '+') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_ADD_ASSIGN;
            }
            return token;
        }
        else if (*curr == '-') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_SUB_ASSIGN;
            }
            return token;
        }
        else if (*curr == '*') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_MUL_ASSIGN;
            }
            return token;
        }
        else if (*curr == '/') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '=') {
                advance();
                token->type = TOK_DIV_ASSIGN;
            }
            return token;
        }
        else if (*curr == '&') {
            produce_token(token, curr, *curr);
            advance();
            if (*curr == '&') {
                advance();
                token->type = TOK_AND;
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
            while (*curr != '\n' && *curr != '\0') {
                curr++;
            }
            continue;
        }
        else if (*curr == '\n') {
            line_num += 1;
            curr += 1;
        }
        else if (*curr == ' ' || *curr == '\t' || *curr == '\r' ||
                 *curr == '\v' || *curr == '\f') {
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

static void produce_token(struct token* tok, char* start_pos, int type) {
    tok->start_pos = start_pos;
    tok->type = type;
    tok->line_num = line_num;
}

static int token_cmp(struct token* tok, char* target, int target_length) {
    if (tok->length != target_length) {
        return 0;
    }
    return (memcmp(tok->start_pos, target, (size_t)tok->length) == 0);
}

static int generate_type(struct token* tok) {
    if (token_cmp(tok, "s8", 2)) {
        return TOK_S8;
    }
    else if (token_cmp(tok, "s16", 3)) {
        return TOK_S16;
    }
    else if (token_cmp(tok, "s32", 3)) {
        return TOK_S32;
    }
    else if (token_cmp(tok, "s64", 3)) {
        return TOK_S64;
    }

    else if (token_cmp(tok, "u8", 2)) {
        return TOK_U8;
    }
    else if (token_cmp(tok, "u16", 3)) {
        return TOK_U16;
    }
    else if (token_cmp(tok, "u32", 3)) {
        return TOK_U32;
    }
    else if (token_cmp(tok, "u64", 3)) {
        return TOK_U64;
    }

    else if (token_cmp(tok, "f32", 3)) {
        return TOK_F32;
    }
    else if (token_cmp(tok, "f64", 3)) {
        return TOK_F64;
    }

    else if (token_cmp(tok, "let", 3)) {
        return TOK_LET;
    }
    else if (token_cmp(tok, "fn", 2)) {
        return TOK_FN;
    }
    else if (token_cmp(tok, "entry", 5)) {
        return TOK_ENTRY;
    }

    else if (token_cmp(tok, "if", 2)) {
        return TOK_IF;
    }
    else if (token_cmp(tok, "else", 4)) {
        return TOK_ELSE;
    }

    else if (token_cmp(tok, "break", 5)) {
        return TOK_BREAK;
    }
    else if (token_cmp(tok, "continue", 8)) {
        return TOK_CONTINUE;
    }

    else if (token_cmp(tok, "return", 6)) {
        return TOK_RETURN;
    }

    else if (token_cmp(tok, "collection", 10)) {
        return TOK_COLLECTION;
    }
    else if (token_cmp(tok, "parent", 6)) {
        return TOK_PARENT;
    }
    else if (token_cmp(tok, "attach", 6)) {
        return TOK_ATTACH;
    }
    else if (token_cmp(tok, "from", 4)) {
        return TOK_FROM;
    }
    else if (token_cmp(tok, "const", 5)) {
        return TOK_CONST;
    }
    else if (token_cmp(tok, "ssize", 5)) {
        return TOK_SSIZE;
    }
    else if (token_cmp(tok, "usize", 5)) {
        return TOK_USIZE;
    }
    else if (token_cmp(tok, "bool", 4)) {
        return TOK_BOOL;
    }
    else if (token_cmp(tok, "string", 6)) {
        return TOK_STRING_TYPE;
    }
    else if (token_cmp(tok, "void", 4)) {
        return TOK_VOID;
    }
    else if (token_cmp(tok, "while", 5)) {
        return TOK_WHILE;
    }
    else if (token_cmp(tok, "struct", 6)) {
        return TOK_STRUCT;
    }
    else if (token_cmp(tok, "interface", 9)) {
        return TOK_INTERFACE;
    }
    else if (token_cmp(tok, "extends", 7)) {
        return TOK_EXTENDS;
    }
    else if (token_cmp(tok, "module", 6)) {
        return TOK_MODULE;
    }
    else if (token_cmp(tok, "implements", 10)) {
        return TOK_IMPLEMENTS;
    }
    else if (token_cmp(tok, "static", 6)) {
        return TOK_STATIC;
    }
    else if (token_cmp(tok, "export", 6)) {
        return TOK_EXPORT;
    }
    else if (token_cmp(tok, "import", 6)) {
        return TOK_IMPORT;
    }

    return TOK_ID;
}

static int is_alpha(int character) {
    return (character >= 'A' && character <= 'Z') ||
           (character >= 'a' && character <= 'z');
}

static int is_num(int character) {
    return (character >= '0' && character <= '9');
}
