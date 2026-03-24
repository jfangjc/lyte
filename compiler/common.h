#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_LET,
    TOK_FN,
    TOK_ENTRY,

    // conditional
    TOK_IF,
    TOK_ELSE,

    // loop
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_RETURN,

    // identifier
    TOK_ID,

    // types
    TOK_S8,
    TOK_S16,
    TOK_S32,
    TOK_S64,
    TOK_U8,
    TOK_U16,
    TOK_U32,
    TOK_U64,
    TOK_F32,
    TOK_F64,

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
    TOK_STRING,

    // language keywords
    TOK_IMPORT,
    TOK_EXPORT,
    TOK_COLLECTION,
    TOK_PARENT,
    TOK_ATTACH,
    TOK_FROM,
    TOK_CONST,
    TOK_SSIZE,
    TOK_USIZE,
    TOK_BOOL,
    TOK_STRING_TYPE,
    TOK_VOID,
    TOK_WHILE,
    TOK_STRUCT,
    TOK_INTERFACE,
    TOK_EXTENDS,
    TOK_MODULE,
    TOK_IMPLEMENTS,
    TOK_STATIC
};

#endif
