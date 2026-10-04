#pragma once

typedef void (*test_func_t)(void);

typedef struct {
    const char* name;
    test_func_t func;
} test_case_t;

#define MAX_TESTS 100
extern test_case_t tests[MAX_TESTS];
extern int test_count;

void register_test(const char* name, test_func_t func);
void test_assert_eq(int expected, int actual, const char* file, int line);
void test_assert_true(int condition, const char* expr, const char* file, int line);
void test_assert_str_eq(const char* expected, const char* actual, const char* file, int line);
