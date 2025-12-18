#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>

void create_temp_file(const char* name, const char* content) {
    FILE* f = fopen(name, "w");
    if (!f) {
        perror("Failed to create temp file");
        exit(1);
    }
    fprintf(f, "%s", content);
    fclose(f);
}
