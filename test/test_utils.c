#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>

void create_temp_file(const char* name, const char* content) {
    FILE* f = NULL;
#ifdef _MSC_VER
    fopen_s(&f, name, "w");
#else
    f = fopen(name, "w");
#endif
    if (!f) {
        perror("Failed to create temp file");
        exit(1);
    }
    fprintf(f, "%s", content);
    fclose(f);
}
