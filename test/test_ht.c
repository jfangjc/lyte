#include "framework.h"
#include "ht.h"
#include "lexer.h"
#include <string.h>

void test_ht_basic(void) {
    struct ht* table = ht_create();
    test_assert_true(table != NULL, "table != NULL", __FILE__, __LINE__);
    test_assert_eq(0, (int)table->size, __FILE__, __LINE__);

    struct token t1;
    t1.start_pos = "key1";
    t1.length = 4;

    ht_insert(table, &t1);
    test_assert_eq(1, (int)table->size, __FILE__, __LINE__);

    struct token* found = ht_lookup(table, &t1);
    test_assert_true(found != NULL, "found != NULL", __FILE__, __LINE__);
    test_assert_true(found == &t1, "found == &t1", __FILE__, __LINE__);

    struct token t2;
    t2.start_pos = "key2";
    t2.length = 4;

    struct token* not_found = ht_lookup(table, &t2);
    test_assert_true(not_found == NULL, "not_found == NULL", __FILE__, __LINE__);

    ht_free(table);
}

void test_ht_expansion(void) {
    struct ht* table = ht_create();
    test_assert_eq(8, (int)table->capacity, __FILE__, __LINE__);

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
    char* keys[] = {"k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9", "k10"};

    for (int i = 0; i < 10; i++) {
        tokens[i].start_pos = keys[i];
        tokens[i].length = (int)strlen(keys[i]);
        ht_insert(table, &tokens[i]);
    }

    test_assert_true(table->capacity > 8, "table->capacity > 8", __FILE__, __LINE__);
    test_assert_eq(10, (int)table->size, __FILE__, __LINE__);

    for (int i = 0; i < 10; i++) {
        struct token* found = ht_lookup(table, &tokens[i]);
        test_assert_true(found != NULL, "found != NULL", __FILE__, __LINE__);
        test_assert_str_eq(keys[i], found->start_pos, __FILE__, __LINE__);
    }

    ht_free(table);
}

void test_ht_collision(void) {
    struct ht* table = ht_create();

    // Force collision if possible, or just rely on FNV properties with enough
    // keys? Hard to force collision without knowing hash function internals
    // perfectly or brute forcing. But we can test that multiple items work.

    struct token t1 = {.start_pos = "abc", .length = 3};
    struct token t2 = {.start_pos = "acb", .length = 3}; // Likely different hash
    struct token t3 = {.start_pos = "bac", .length = 3};

    ht_insert(table, &t1);
    ht_insert(table, &t2);
    ht_insert(table, &t3);

    test_assert_eq(3, (int)table->size, __FILE__, __LINE__);
    test_assert_true(ht_lookup(table, &t1) == &t1, "ht_lookup(table, &t1) == &t1", __FILE__, __LINE__);
    test_assert_true(ht_lookup(table, &t2) == &t2, "ht_lookup(table, &t2) == &t2", __FILE__, __LINE__);
    test_assert_true(ht_lookup(table, &t3) == &t3, "ht_lookup(table, &t3) == &t3", __FILE__, __LINE__);

    ht_free(table);
}
