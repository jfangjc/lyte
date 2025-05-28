#include "gen.h"

#include <stdlib.h>

#include "parser.h"
#include "emitter.h"

void gen_fn(struct fn_decl* fn_decl);

void gen(char* bin_name, struct program_ast* program) {
    emitter(bin_name);
    struct fn_decl* fn_decl = program->fn_decls;
    while (fn_decl != NULL) {
        gen_fn(fn_decl);
        fn_decl = fn_decl->next;
    }
    emit_finish();
}

void gen_fn(struct fn_decl* fn_decl) {
    emit_label(fn_decl->name->start_pos, fn_decl->name->length);
}

