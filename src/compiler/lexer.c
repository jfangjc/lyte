#include "lexer.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define macro

char* read_file(char* Path) {
    unsigned long long Size = 0;
    char* Content;

    FILE* File;
	fopen_s(&File, Path, "r");

    if (File != NULL) {
        fseek(File, 0, SEEK_END);
        Size = ftell(File);
        fseek(File, 0, SEEK_SET);

        if (Size > 0) {
            Content = (char*)malloc(Size + 1);
            fread(Content, Size, 1, File);
            fclose(File);
            Content[Size] = '\0';

            return Content;
        }
        return (char*)-2;
    }
    return (char*)-1;
}

static int line_num = 1;
static int char_num = 0;

static int ischar(char src) {
    if ((src >= 'A' && src <= 'Z') || (src >= 'a' && src <= 'z') || src == '_') {
        return 1;
    }
    return 0;
}

static int isnum(char src) {
    if ((src >= '0' && src <= '9') || src == '.') {
        return 1;
    }
    return 0;
}

static int issymbol(char src) {
	// Special symbols need to be considered: &&, ||, ^^, +=, -=, *=, /=, <<, >>
	if (src < '0' || (src > '9' && src < 'A') || (src > 'Z' && src < 'a') || src > 'z') {
		return 1;
	}
	return 0;
}

static int word_type(char* src) {
    if (!strcmp(src, "si8")) { return TOK_SI8; }
    else if (!strcmp(src, "si16")) { return TOK_SI16; }
    else if (!strcmp(src, "si32")) { return TOK_SI32; }
    else if (!strcmp(src, "si64")) { return TOK_SI64; }
    else if (!strcmp(src, "si128")) { return TOK_SI128; }
    else if (!strcmp(src, "ui8")) { return TOK_UI8; }
    else if (!strcmp(src, "ui16")) { return TOK_UI16; }
    else if (!strcmp(src, "ui32")) { return TOK_UI32; }
    else if (!strcmp(src, "ui64")) { return TOK_UI64; }
    else if (!strcmp(src, "ui128")) { return TOK_UI128; }
    // implement more complex float type later
    else if (!strcmp(src, "float")) { return TOK_FLOAT; }
    else if (!strcmp(src, "double")) { return TOK_DFLOAT; }
    else if (!strcmp(src, "type")) { return TOK_TYPE; }
    else if (!strcmp(src, "fn")) { return TOK_FUNCTION; }
    else if (!strcmp(src, "asm")) { return TOK_ASM; }
	return TOK_IDENTIFIER;
}

struct token next(char* src, int* index) {
	// each token should not be longer than 128 characters
	while (src[*index] == '\n' || src[*index] == ' ' || (src[*index] == '/' && (src[*index + 1] == '/' || src[*index + 1] == '*'))) {
		if (src[*index] == '\n') {
			line_num += 1;
			char_num = 0;
			*index += 1;
		}
		if (src[*index] == ' ') {
			char_num += 1;
			*index += 1;
		}
		if (src[*index] == '/' && src[*index + 1] == '/') {
			while (src[*index] != '\n') {
				*index += 1;
				char_num += 1;
			}
		}
		if (src[*index] == '/' && src[*index + 1] == '*') {
			while (src[*index - 1] != '*' || src[*index] != '/') {
				*index += 1;
				char_num += 1;
				if (src[*index] == '\n') {
					line_num += 1;
					char_num = 0;
				}
			}
			*index += 1;
			char_num += 1;
		}
	}

	struct token token;
	char* buffer = malloc(sizeof(char) * 128);
	int count = 0;
	// string
	if (ischar(src[*index])) {
		while (ischar(src[*index]) || isnum(src[*index])) {
			buffer[count] = src[*index];
			char_num += 1;
			count += 1;
			*index += 1;
		}
		buffer[count] = '\0';
		token.type = word_type(buffer);
	}
	// number
	else if (isnum(src[*index])) {
	    token.type = TOK_INT;
		while (isnum(src[*index])) {
		    if (src[*index] == '.') {
		        token.type = TOK_DECIMAL;
			}
		    buffer[count] = src[*index];
			char_num += 1;
			count += 1;
			*index += 1;
		}
		buffer[count] = '\0';

	}
	// dealing with character
	else if (src[*index] == '\'') {
		for (int i = 0; i < 3; i++) {
			buffer[count] = src[*index];
			char_num += 1;
			count += 1;
			*index += 1;
		}
		if (src[*index - 1] != '\'') {
			// the character type length is longer than 1
			exit(-1);
		}
		buffer[count] = '\0';
		token.type = TOK_CHAR;
	}
	// dealing with string
	// need to realloc the buffer here
	else if (src[*index] == '"') {
	    free(buffer);
		buffer = malloc(sizeof(char) * MAX_STRING_SIZE);
		buffer[count] = src[*index];
		char_num += 1;
		count += 1;
		*index += 1;
		while (src[*index] != '"') {
			buffer[count] = src[*index];
			char_num += 1;
			count += 1;
			*index += 1;
		}
		buffer[count] = src[*index];
		char_num += 1;
		count += 1;
		*index += 1;
		buffer[count] = '\0';
		token.type = TOK_STRING;
	}
	// other symbols
	else if (issymbol(src[*index])) {
		buffer[count] = src[*index];
		char_num += 1;
		count += 1;
		*index += 1;
		buffer[count] = '\0';
		token.type = (int)src[*index - 1];
	}
	// unrecognized symbols, returns an error
	else {
	    exit(-1);
	}

	token.value = malloc(strlen(buffer) * sizeof(char));
	memcpy(token.value, buffer, strlen(buffer) * sizeof(char));
	token.value[strlen(buffer)] = '\0';
	free(buffer);

	token.char_num = char_num;
	token.line_num = line_num;
	return token;
}
