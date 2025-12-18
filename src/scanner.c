#include "scanner.h"
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
            else if (*curr == '/') {
                // Comment
                while (*curr != '\n' && *curr != '\0') {
                    curr++;
                }
                // Don't return a token, loop again
                // But next_token structure is a big while loop.
                // We need to continue the outer loop.
                // However, we already called produce_token and allocated
                // 'token'. If we loop again, we need to reset 'token'.
                // Actually, the loop is `while (*curr != '\0')`.
                // If we hit a comment, we consume it.
                // Then we need to restart finding the next token.
                // But we already did `produce_token` which sets start_pos.
                // If we just continue, the next iteration will call
                // `produce_token` again? No, `produce_token` is called inside
                // each if/else block. So if we are here, we consumed `/`. If it
                // is a comment, we consume until newline. Then we are at
                // newline or EOF. If we just `continue` the while loop, it will
                // handle newline or whatever next. But we need to NOT return
                // the token we started producing. So we should NOT return
                // So we should NOT return `token` here.
                token->length = 0;
                continue;
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
            while (*(curr++) != '\n') {
            }
            line_num += 1;
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
    if (token_cmp(tok, "s8", 2) || token_cmp(tok, "i8", 2)) {
        return TOK_S8;
    }
    else if (token_cmp(tok, "s16", 3) || token_cmp(tok, "i16", 3)) {
        return TOK_S16;
    }
    else if (token_cmp(tok, "s32", 3) || token_cmp(tok, "i32", 3)) {
        return TOK_S32;
    }
    else if (token_cmp(tok, "s64", 3) || token_cmp(tok, "i64", 3)) {
        return TOK_S64;
    }
    else if (token_cmp(tok, "s128", 4) || token_cmp(tok, "i128", 4)) {
        return TOK_S128;
    }

    else if (token_cmp(tok, "u8", 2) || token_cmp(tok, "ui8", 3)) {
        return TOK_U8;
    }
    else if (token_cmp(tok, "u16", 3) || token_cmp(tok, "ui16", 4)) {
        return TOK_U16;
    }
    else if (token_cmp(tok, "u32", 3) || token_cmp(tok, "ui32", 4)) {
        return TOK_U32;
    }
    else if (token_cmp(tok, "u64", 3) || token_cmp(tok, "ui64", 4)) {
        return TOK_U64;
    }
    else if (token_cmp(tok, "u128", 4) || token_cmp(tok, "ui128", 5)) {
        return TOK_U128;
    }

    else if (token_cmp(tok, "f32", 3)) {
        return TOK_F32;
    }
    else if (token_cmp(tok, "f64", 3)) {
        return TOK_F64;
    }
    else if (token_cmp(tok, "f128", 4)) {
        return TOK_F128;
    }

    else if (token_cmp(tok, "let", 3)) {
        return TOK_LET;
    }
    else if (token_cmp(tok, "fn", 2)) {
        return TOK_FN;
    }

    else if (token_cmp(tok, "addr", 4)) {
        return TOK_ADDR;
    }
    else if (token_cmp(tok, "mut", 3)) {
        return TOK_MUT;
    }
    else if (token_cmp(tok, "define", 6)) {
        return TOK_DEFINE;
    }

    else if (token_cmp(tok, "if", 2)) {
        return TOK_IF;
    }
    else if (token_cmp(tok, "else", 4)) {
        return TOK_ELSE;
    }

    else if (token_cmp(tok, "for", 3)) {
        return TOK_FOR;
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

    return TOK_ID;
}

static int is_alpha(int character) {
    return (character >= 'A' && character <= 'Z') ||
           (character >= 'a' && character <= 'z');
}

static int is_num(int character) {
    return (character >= '0' && character <= '9');
}
