#ifndef COMPILER_TOKEN
#define COMPILER_TOKEN

enum TOKENS {
    // EOF
    TOK_EOF = 256,

    // keywords
    TOK_VAR, TOK_CONST, TOK_ADDR, TOK_FN, TOK_RETURN, // 261
    // condition
    TOK_IF, TOK_ELSEIF, TOK_ELSE, // 264
    // loop
    TOK_LOOP, TOK_WHILE, TOK_FOR, TOK_IN, TOK_TO, TOK_BREAK, TOK_CONTINUE, // 271
    // identifier
    TOK_ID, // 272

    // types
    TOK_SI8, TOK_SI16, TOK_SI32, TOK_SI64, // 276
    TOK_UI8, TOK_UI16, TOK_UI32, TOK_UI64, // 280
    TOK_NULL, //281
    
    // compare
    TOK_EQEQ, TOK_NOTEQ, TOK_GTEQ, TOK_LTEQ, // 285

    // logic
    TOK_OROR, TOK_ANDAND, // 287

    // values
    TOK_NUM, TOK_CHAR, TOK_STRING, // 290

    // others
    TOK_IMPORT, TOK_EXPORT // 292
};

#endif
