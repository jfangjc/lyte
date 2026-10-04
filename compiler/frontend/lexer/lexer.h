#ifndef COMPILER_LEXER
#define COMPILER_LEXER

struct token {
    char* start_pos;
    int length;
    int type;
    int line_num;
};

struct lexer {
    char* curr;
    int line_num;
    int col_num;
};

void lexer_init(struct lexer* lex, char* source);

char* read_file(char* path);

struct token* next_token(void);

#endif
