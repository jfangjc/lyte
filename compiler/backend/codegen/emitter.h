#ifndef COMPILER_EMITTER
#define COMPILER_EMITTER

void emitter(const char* file_name);

void emit_finish(void);

void emit(const char* op);

void emit_label(const char* start, int len);

#endif
