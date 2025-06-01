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
    TOK_S8, TOK_S16, TOK_S32, TOK_S64, TOK_S128, // 272
    TOK_U8, TOK_U16, TOK_U32, TOK_U64, TOK_U128, // 276
    TOK_F32, TOK_F64, TOK_F128, // 279
    
    //assign 
    TOK_ASSIGN, // 280

    // compare
    TOK_EQEQ, TOK_NOTEQ, TOK_GTEQ, TOK_LTEQ, // 284

    // logic
    TOK_OR, TOK_AND, TOK_NOT, // 287

    // values
    TOK_INT, TOK_FLOAT, TOK_CHAR, TOK_STRING, // 291

    // others
    TOK_IMPORT, TOK_EXPORT // 293
};

#endif
