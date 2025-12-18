#include "framework.h"
#include <stdio.h>

test_case_t tests[MAX_TESTS];
int test_count = 0;

int main(void) {
    printf("Running %d tests...\n", test_count);
    int passed = 0;
    for (int i = 0; i < test_count; i++) {
        printf("[RUNNING] %s\n", tests[i].name);
        tests[i].func();
        printf("[PASSED]  %s\n", tests[i].name);
        passed++;
    }
    printf("\n%d/%d tests passed.\n", passed, test_count);
    return 0;
}
