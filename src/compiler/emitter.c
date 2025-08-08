#include "emitter.h"

#include <stdio.h>

static FILE* target;

void emitter(char* file_name) {
    target = fopen(file_name, "w");
}

void emit_finish() {
    fclose(target);
}

void emit(char* op) {
    fprintf(target, "%s", op);
}

void emit_label(char* start, int len) {
    for (int i = 0; i < len; i++) {
        fprintf(target, "%c", start[i]);
    }
    fprintf(target, ":\n");
}
