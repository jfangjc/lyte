#include "scanner.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

// define macro
#define MAX_STRING_SIZE 128

enum letter_type {
	empty = 0, string, number, symbol, comment, other
};

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
	if ((src != '_' && src != '#') && ((src > ' ' && src < '0') || (src > '9' && src < 'A') || (src > 'Z' && src < 'a') || src > 'z')) {
		return 1;
	}
	return 0;
}

static int iscomment(char src) {
    if (src == '#') {
        return 1;
    }
    return 0;
}

static int iswhitespace(char src) {
    if (src == ' ' || src == '\r' || src == '\t') {
        line_num += 1;
        return 1;
    }
    if (src == '\n') {
        line_num += 1;
        char_num = 0;
        return 1;
    }
    return 0;
}

char* read_file(char* path) {
    FILE* File = fopen(path, "r");

    if (File) {
        fseek(File, 0, SEEK_END);
        unsigned long long Size = ftell(File);
        fseek(File, 0, SEEK_SET);

        if (Size) {
			char* Content = (char*)malloc(Size + 1);
            fread(Content, Size, 1, File);
            fclose(File);
            Content[Size] = '\0';
            return Content;
        }
        error(1);
        return NULL;
    }
    error(2);
    return NULL;
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

    else if (!strcmp(src, "import")) { return TOK_IMPORT; }
    else if (!strcmp(src, "export")) { return TOK_EXPORT; }
	return TOK_IDENTIFIER;
}

struct token *next(char *src) {
    int type = -1;
    int count = 0;
    char *temp = malloc(MAX_STRING_SIZE);
    char curr = *src;
    struct token *head = malloc(sizeof(struct token));
    head -> value = NULL;
    struct token *curr_token = head;

	while (curr != '\0') {
        if ((type == 1 && !ischar(curr) && !isnum(curr)) || (type == 2 && !isnum(curr))) {
            temp[count] = '\0';
            curr_token -> value = malloc((count + 1) * sizeof(char));
            memcpy(curr_token->value, temp, count + 1);
            curr_token -> type = word_type(curr_token -> value);
            curr_token -> next = malloc(sizeof(struct token));
            if (type == 1) {
                curr_token->type = word_type(temp);
            }
            curr_token = curr_token -> next;
            curr_token -> value = NULL;
            count = 0;
        }

        if (ischar(curr)) {
            type = 1;
        }
        else if (isnum(curr)) {
            type = 2;
        }
        else if (issymbol(curr)) {
            type = -1;
            temp[0] = curr;
            temp[1] = '\0';
            curr_token -> value = malloc(2 * sizeof(char));
            memcpy(curr_token->value, temp, 2);
            curr_token -> type = (int)curr;
            curr_token -> next = malloc(sizeof(struct token));
            curr_token = curr_token -> next;
            curr_token -> value = NULL;
            count = 0;
            curr = *src++;
            continue;
        }
        else if (iscomment(curr)) {
            type = -1;
            while (*src++ != '\n') {
            }
            curr = *src++;
            count = 0;
            continue;
        }
        else if (iswhitespace(curr)) {
            type = -1;
            curr = *src++;
            count = 0;
            continue;
        }
        else {
            error(3);
        }

        temp[count] = curr;
        count += 1;
        char_num += 1;
        curr = *src++;
	}
    return head;
}
