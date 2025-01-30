#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_VAR, TOK_CONST, TOK_FUNCTION, TOK_RETURN,

    // identifier
    TOK_IDENTIFIER,

    // types
    TOK_SI8, TOK_SI16, TOK_SI32, TOK_SI64, TOK_UI8, TOK_UI16, TOK_UI32, TOK_UI64, TOK_NULL,
    
    // compare
    TOK_EQUAL, TOK_NOTEQUAL, TOK_GREATEREQUAL, TOK_LESSEQUAL,

    // values
    TOK_ID, TOK_NUM,

    // others
    TOK_IMPORT, TOK_EXPORT
};

#endif
