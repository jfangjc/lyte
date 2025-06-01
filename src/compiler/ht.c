#include "ht.h"

#include <stdlib.h>

#include "scanner.h"

void ht_expand(struct ht* ht);

unsigned int hash(char* item, int len) {
    unsigned int hash = fnv_offset_basis;
    for (int i = 0; i < len; i++) {
        hash ^= (unsigned char)item[i];
        hash *= fnv_prime;
    }
    return hash;
}

struct ht* ht_create() {
    struct ht* ht = malloc(sizeof(struct ht));

    ht->size = 0;
    ht->capacity = ht_init_capacity;
    ht->entries = malloc(ht->capacity * sizeof(struct entry));

    for (int i = 0; i < ht_init_capacity; i++) {
        ht->entries[i].value = NULL;
    }

    return ht;
}

unsigned int ht_insert(struct ht* ht, struct token* value) {
    unsigned int key = hash(value->start_pos, value->length);
    int index = (key & (unsigned int)(ht->capacity - 1));

    if (ht->size >= ht->capacity * 0.8) {
        ht_expand(ht);
    }

    while (ht->entries[index].value != NULL) {
        index += 1;
        if (index >= ht->capacity) {
            index = 0;
        }
    }

    ht->entries[index].key = key;
    ht->entries[index].value = value;
    ht->size += 1;
    return key;
}

struct token* ht_lookup(struct ht* ht, struct token* value) {
    unsigned int key = hash(value->start_pos, value->length);
    int index = (key & (unsigned int)(ht->capacity - 1));

    while (ht->entries[index].value->length != value->length
        || ht->entries[index].key != key) {
        index += 1;
        if (index >= ht->capacity) {
            index = 0;
        }
    }

    return ht->entries[index].value;
}

void ht_expand(struct ht* ht) {
    int old_capacity = ht->capacity;
    struct entry* old_entries = ht->entries;

    ht->capacity = ht->capacity * 2;
    ht->size = 0;
    ht->entries = malloc(ht->capacity * sizeof(struct entry));

    for (int i = 0; i < ht->capacity; i++) {
        ht->entries[i].value = NULL;
    }

    for (int i = 0; i < old_capacity; i++) {
        if (old_entries[i].value != NULL) {
            ht_insert(ht, old_entries[i].value);
            old_entries[i].value = NULL;
        }
    }

    free(old_entries);
}