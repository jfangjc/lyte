#include "../src/ht.h"
#include "../src/scanner.h"
#include "framework.h"
#include <stdio.h>
#include <stdlib.h>

TEST(ht_basic) {
    struct ht* table = ht_create();
    ASSERT_TRUE(table != NULL);
    ASSERT_EQ(0, table->size);

    struct token t1;
    t1.start_pos = "key1";
    t1.length = 4;

    ht_insert(table, &t1);
    ASSERT_EQ(1, table->size);

    struct token* found = ht_lookup(table, &t1);
    ASSERT_TRUE(found != NULL);
    ASSERT_TRUE(found == &t1);

    struct token t2;
    t2.start_pos = "key2";
    t2.length = 4;

    struct token* not_found = ht_lookup(table, &t2);
    ASSERT_TRUE(not_found == NULL);

    ht_free(table);
}

TEST(ht_expansion) {
    struct ht* table = ht_create();
    ASSERT_EQ(8, table->capacity);

    // Insert 8 items to trigger expansion (threshold is 0.8 * 8 = 6.4 -> 7
    // items) Actually code says >= capacity * 0.8. 8 * 0.8 = 6.4. So 7th item
    // triggers? Let's check ht.c: if (ht->size >= ht->capacity * 0.8)
    // ht_expand(ht); size is incremented AFTER this check. Insert 1: size 0
    // < 6.4. size becomes 1.
    // ...
    // Insert 6: size 5 < 6.4. size becomes 6.
    // Insert 7: size 6 < 6.4. size becomes 7.
    // Insert 8: size 7 >= 6.4. EXPAND. size becomes 8.

    struct token tokens[10];
    char* keys[] = {"k1", "k2", "k3", "k4", "k5",
                    "k6", "k7", "k8", "k9", "k10"};

    for (int i = 0; i < 10; i++) {
        tokens[i].start_pos = keys[i];
        tokens[i].length = (int)strlen(keys[i]);
        ht_insert(table, &tokens[i]);
    }

    ASSERT_TRUE(table->capacity > 8);
    ASSERT_EQ(10, table->size);

    for (int i = 0; i < 10; i++) {
        struct token* found = ht_lookup(table, &tokens[i]);
        ASSERT_TRUE(found != NULL);
        ASSERT_STR_EQ(keys[i], found->start_pos);
    }

    ht_free(table);
}

TEST(ht_collision) {
    struct ht* table = ht_create();

    // Force collision if possible, or just rely on FNV properties with enough
    // keys? Hard to force collision without knowing hash function internals
    // perfectly or brute forcing. But we can test that multiple items work.

    struct token t1 = {.start_pos = "abc", .length = 3};
    struct token t2 = {.start_pos = "acb",
                       .length = 3}; // Likely different hash
    struct token t3 = {.start_pos = "bac", .length = 3};

    ht_insert(table, &t1);
    ht_insert(table, &t2);
    ht_insert(table, &t3);

    ASSERT_EQ(3, table->size);
    ASSERT_TRUE(ht_lookup(table, &t1) == &t1);
    ASSERT_TRUE(ht_lookup(table, &t2) == &t2);
    ASSERT_TRUE(ht_lookup(table, &t3) == &t3);

    ht_free(table);
}
