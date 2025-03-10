#ifndef COMPILER_LEXER
#define COMPILER_LEXER

struct token {
    char* start_pos;
    int length;
    int type;
    int line_num;
};

char* read_file(char* path);
struct token *next_token();

#endif
