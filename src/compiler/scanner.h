#ifndef COMPILER_LEXER
#define COMPILER_LEXER

union token_value {
    char* string;
    unsigned long long integer;
    double decimals;
};

struct token {
    char* value;
    int type;
    int line_num;
    int col_num;
    struct token* next;
};

char* read_file(char* path);
struct token *next(char* src);

#endif
