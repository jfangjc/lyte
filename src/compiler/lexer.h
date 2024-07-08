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
    int char_num;
    //struct token* Next;
};

char* read_file(char* Path);
struct token next(char* src, int* index);

#endif
