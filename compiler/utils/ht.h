#ifndef COMPILER_HT
#define COMPILER_HT

#include "lexer.h"

#define fnv_offset_basis 2166136261
#define fnv_prime 16777619

#define ht_init_capacity 8

struct entry {
    unsigned int key;
    struct token* value;
};

struct ht {
    struct entry* entries;
    unsigned int size;
    unsigned int capacity;
};

unsigned int hash(char* item, int len);

struct ht* ht_create(void);

unsigned int ht_insert(struct ht* ht, struct token* value);

struct token* ht_lookup(struct ht* ht, struct token* value);

int ht_free(struct ht* ht);

void ht_expand(struct ht* ht);

#endif
