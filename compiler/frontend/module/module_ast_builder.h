#ifndef COMPILER_MODULE_AST_BUILDER
#define COMPILER_MODULE_AST_BUILDER

#include "parser.h"

struct module_ast_builder {
    struct program_ast program;
    struct import_decl** imports_tail;
    struct export_decl** exports_tail;
    struct type_decl** types_tail;
    struct var_decl** vars_tail;
    struct fn_decl** fns_tail;
};

void module_ast_builder_init(struct module_ast_builder* builder, char* module_name);
void module_ast_builder_append(struct module_ast_builder* builder, struct program_ast* file_program);
void module_ast_validate_exports(struct program_ast* program);

#endif
