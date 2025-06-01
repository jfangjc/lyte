#include "error.h"

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

void error(char *error_msg) {
    fprintf(stderr, "%s\n", error_msg);
    exit(0);
}
