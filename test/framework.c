#include "framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

test_case_t tests[MAX_TESTS];
int test_count = 0;

void register_test(const char* name, test_func_t func) {
    if (test_count < MAX_TESTS) {
        tests[test_count].name = name;
        tests[test_count].func = func;
        test_count++;
    }
    else {
        fprintf(stderr, "Max tests reached!\n");
    }
}

void test_assert_eq(int expected, int actual, const char* file, int line) {
    if (expected != actual) {
        fprintf(stderr, "Assertion failed: %d == %d\n", expected, actual);
        fprintf(stderr, "  Expected: %d\n", expected);
        fprintf(stderr, "  Actual:   %d\n", actual);
        fprintf(stderr, "  File:     %s:%d\n", file, line);
        exit(1);
    }
}

void test_assert_true(int condition, const char* expr, const char* file, int line) {
    if (!condition) {
        fprintf(stderr, "Assertion failed: %s\n", expr);
        fprintf(stderr, "  File:     %s:%d\n", file, line);
        exit(1);
    }
}

void test_assert_str_eq(const char* expected, const char* actual, const char* file, int line) {
    if (strcmp(expected, actual) != 0) {
        fprintf(stderr, "Assertion failed: strcmp(\"%s\", \"%s\") == 0\n", expected, actual);
        fprintf(stderr, "  Expected: \"%s\"\n", expected);
        fprintf(stderr, "  Actual:   \"%s\"\n", actual);
        fprintf(stderr, "  File:     %s:%d\n", file, line);
        exit(1);
    }
}
