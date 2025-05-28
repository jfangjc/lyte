#ifndef COMPILER_EMITTER
#define COMPILER_EMITTER

void emitter(char* file_name);

void emit_finish();

void emit(char* op);

void emit_label(char* start, int len);

#endif