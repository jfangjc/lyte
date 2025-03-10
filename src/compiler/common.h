#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_VAR, TOK_CONST, TOK_ADDR, TOK_FN, TOK_RETURN,
    // condition
    TOK_IF, TOK_ELSEIF, TOK_ELSE,
    // loop
    TOK_LOOP, TOK_WHILE, TOK_FOR, TOK_IN, TOK_TO, TOK_BREAK, TOK_CONTINUE,
    // identifier
    TOK_ID,

    // types
    TOK_SI8, TOK_SI16, TOK_SI32, TOK_SI64,
    TOK_UI8, TOK_UI16, TOK_UI32, TOK_UI64,
    TOK_NULL,
    
    // compare
    TOK_EQEQ, TOK_NOTEQ, TOK_GTEQ, TOK_LTEQ,

    // logic
    TOK_OROR, TOK_ANDAND,

    // values
    TOK_NUM, TOK_CHAR, TOK_STRING,

    // others
    TOK_IMPORT, TOK_EXPORT
};

#endif
