#include "keywords.h"
#include "common.h"
#include "ht.h"

#include <string.h>

struct keyword_entry {
    const char* name;
    int len;
    int tok_type;
};

#define KEYWORD_TABLE_SIZE 32
#define KEYWORD_TABLE_MASK (KEYWORD_TABLE_SIZE - 1)

static struct keyword_entry keyword_table[KEYWORD_TABLE_SIZE];
static int keyword_table_initialised = 0;

static const struct keyword_entry keyword_list[] = {
    {"break", 5, TOK_BREAK},
    {"const", 5, TOK_CONST},
    {"continue", 8, TOK_CONTINUE},
    {"else", 4, TOK_ELSE},
    {"export", 6, TOK_EXPORT},
    {"fn", 2, TOK_FN},
    {"for", 3, TOK_FOR},
    {"if", 2, TOK_IF},
    {"import", 6, TOK_IMPORT},
    {"module", 6, TOK_MODULE},
    {"return", 6, TOK_RETURN},
    {"type", 4, TOK_TYPE},
    {"unsafe", 6, TOK_UNSAFE},
    {"var", 3, TOK_VAR},
};

static const int keyword_count = (int)(sizeof(keyword_list) / sizeof(keyword_list[0]));

static void keyword_table_init(void) {
    for (int i = 0; i < keyword_count; i++) {
        unsigned int index = hash((char*)keyword_list[i].name, keyword_list[i].len)
                           & KEYWORD_TABLE_MASK;
        while (keyword_table[index].name != NULL) {
            index = (index + 1) & KEYWORD_TABLE_MASK;
        }
        keyword_table[index] = keyword_list[i];
    }
    keyword_table_initialised = 1;
}

int keyword_lookup(const char* text, int length) {
    if (!keyword_table_initialised) {
        keyword_table_init();
    }

    if (length < 2 || length > 8) {
        return TOK_ID;
    }

    unsigned int index = hash((char*)text, length) & KEYWORD_TABLE_MASK;

    while (1) {
        const struct keyword_entry* entry = &keyword_table[index];

        if (entry->name == NULL) {
            return TOK_ID;
        }

        if (entry->len == length &&
            memcmp(text, entry->name, (size_t)length) == 0) {
            return entry->tok_type;
        }

        index = (index + 1) & KEYWORD_TABLE_MASK;
    }
}
