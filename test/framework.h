#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*test_func_t)(void);

typedef struct {
    const char* name;
    test_func_t func;
} test_case_t;

#define MAX_TESTS 100
extern test_case_t tests[MAX_TESTS];
extern int test_count;

#define TEST(test_name)                                                        \
    void test_##test_name(void);                                               \
    __attribute__((constructor)) void register_##test_name(void) {             \
        if (test_count < MAX_TESTS) {                                          \
            tests[test_count].name = #test_name;                               \
            tests[test_count].func = test_##test_name;                         \
            test_count++;                                                      \
        }                                                                      \
    }                                                                          \
    void test_##test_name(void)

#define ASSERT_EQ(expected, actual)                                            \
    do {                                                                       \
        if ((expected) != (actual)) {                                          \
            fprintf(stderr, "Assertion failed: %s == %s\n", #expected,         \
                    #actual);                                                  \
            fprintf(stderr, "  Expected: %d\n", (int)(expected));              \
            fprintf(stderr, "  Actual:   %d\n", (int)(actual));                \
            fprintf(stderr, "  File:     %s:%d\n", __FILE__, __LINE__);        \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "Assertion failed: %s\n", #condition);             \
            fprintf(stderr, "  File:     %s:%d\n", __FILE__, __LINE__);        \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

#define ASSERT_STR_EQ(expected, actual)                                        \
    do {                                                                       \
        if (strcmp((expected), (actual)) != 0) {                               \
            fprintf(stderr, "Assertion failed: strcmp(%s, %s) == 0\n",         \
                    #expected, #actual);                                       \
            fprintf(stderr, "  Expected: \"%s\"\n", (expected));               \
            fprintf(stderr, "  Actual:   \"%s\"\n", (actual));                 \
            fprintf(stderr, "  File:     %s:%d\n", __FILE__, __LINE__);        \
            exit(1);                                                           \
        }                                                                      \
    } while (0)
