#include "keywords.h"
#include "common.h"

#include <string.h>

struct keyword_entry {
    char* name;
    int len;
    int tok_type;
};

// Keep entries sorted lexicographically for binary search.
static const struct keyword_entry keyword_list[] = {
    {"as", 2, TOK_AS},
    {"break", 5, TOK_BREAK},
    {"continue", 8, TOK_CONTINUE},
    {"else", 4, TOK_ELSE},
    {"export", 6, TOK_EXPORT},
    {"false", 5, TOK_FALSE},
    {"fn", 2, TOK_FN},
    {"for", 3, TOK_FOR},
    {"from", 4, TOK_FROM},
    {"if", 2, TOK_IF},
    {"import", 6, TOK_IMPORT},
    {"let", 3, TOK_LET},
    {"match", 5, TOK_MATCH},
    {"module", 6, TOK_MODULE},
    {"mut", 3, TOK_MUT},
    {"new", 3, TOK_NEW},
    {"ref", 3, TOK_REF},
    {"return", 6, TOK_RETURN},
    {"take", 4, TOK_TAKE},
    {"transparent", 11, TOK_TRANSPARENT},
    {"true", 4, TOK_TRUE},
    {"type", 4, TOK_TYPE},
    {"unsafe", 6, TOK_UNSAFE},
    {"var", 3, TOK_VAR},
};

int keyword_lookup(const char* text, int length) {
    if (length < 2 || length > 11) {
        return TOK_ID;
    }

    // Use binary search to match keyword
    int low = 0;
    int high = 24; // total number of keyword
    while (low < high) {
        int mid = low + (high - low) / 2;
        const struct keyword_entry* entry = &keyword_list[mid];

        int cmp = memcmp(text, entry->name, (size_t)(length < entry->len ? length : entry->len));
        if (cmp == 0) {
            cmp = length - entry->len;
            if (cmp == 0) {
                return entry->tok_type;
            }
        }

        if (cmp < 0) {
            high = mid;
        }
        else {
            low = mid + 1;
        }
    }

    return TOK_ID;
}
