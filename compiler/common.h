#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_MODULE,
    TOK_EXPORT,
    TOK_IMPORT,
    TOK_AS,
    TOK_TRANSPARENT,
    TOK_TYPE,
    TOK_FN,
    TOK_HEAP,
    TOK_NEW,
    TOK_LET,
    TOK_VAR,
    TOK_MUT,
    TOK_TAKE,
    TOK_FROM,
    TOK_UNSAFE,
    TOK_FOR,

    // conditional
    TOK_IF,
    TOK_ELSE,
    TOK_MATCH,

    // loop
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_RETURN,
    TOK_TRUE,
    TOK_FALSE,

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

    // match arms and ranges
    TOK_FAT_ARROW,
    TOK_RANGE,

    // values
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,
    TOK_CHAR
};

#endif
