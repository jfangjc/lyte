#include "error.h"

#include <stdio.h>

void error(int error_code){
    fprintf(stderr, "%i, \n", error_code);
}