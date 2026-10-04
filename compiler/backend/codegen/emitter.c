#include "emitter.h"

#include <stdio.h>

static FILE* target;

void emitter(const char* file_name) {
#ifdef _MSC_VER
    fopen_s(&target, file_name, "w");
#else
    target = fopen(file_name, "w");
#endif
}

void emit_finish(void) { fclose(target); }

void emit(const char* op) { fprintf(target, "%s", op); }

void emit_label(const char* start, int len) {
    for (int i = 0; i < len; i++) {
        fprintf(target, "%c", start[i]);
    }
    fprintf(target, ":\n");
}
