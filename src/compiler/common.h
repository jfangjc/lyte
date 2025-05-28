#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_LET, TOK_FN, TOK_VAR, TOK_ADDR, // 260
    // conditional
    TOK_IF, TOK_ELSE, // 262
    // loop
    TOK_FOR, TOK_BREAK, TOK_CONTINUE, TOK_RETURN,// 266
    // identifier
    TOK_ID, // 267

    // types
    TOK_SI8, TOK_SI16, TOK_SI32, TOK_SI64, // 271
    TOK_UI8, TOK_UI16, TOK_UI32, TOK_UI64, // 275
    
    //assign 
    TOK_ASSIGN, // 276

    // compare
    TOK_EQEQ, TOK_NOTEQ, TOK_GTEQ, TOK_LTEQ, // 280

    // logic
    TOK_OR, TOK_AND, TOK_NOT, // 283

    // values
    TOK_INT, TOK_FLOAT, TOK_CHAR, TOK_STRING, // 287

    // others
    TOK_IMPORT, TOK_EXPORT // 289
};

#endif
