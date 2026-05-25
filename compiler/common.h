#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_MODULE,
    TOK_EXPORT,
    TOK_IMPORT,
    TOK_TYPE,
    TOK_FN,
    TOK_CONST,
    TOK_VAR,
    TOK_UNSAFE,
    TOK_FOR,

    // conditional
    TOK_IF,
    TOK_ELSE,

    // loop
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_RETURN,

    // identifier
    TOK_ID,

    // assign
    TOK_ASSIGN,
    TOK_ADD_ASSIGN,
    TOK_SUB_ASSIGN,
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,

    // compare
    TOK_EQEQ,
    TOK_NOTEQ,
    TOK_GTEQ,
    TOK_LTEQ,

    // logic
    TOK_OR,
    TOK_AND,
    TOK_NOT,

    // values
    TOK_INT,
    TOK_FLOAT,
    TOK_CHAR,
    TOK_STRING
};

#endif
