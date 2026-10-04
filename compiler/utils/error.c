#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void error(const char* error_msg) {
    fprintf(stderr, "%s\n", error_msg);
    exit(EXIT_FAILURE);
}
