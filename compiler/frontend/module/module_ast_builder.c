#include "module_ast_builder.h"

#include "error.h"

#include <stddef.h>
#include <string.h>

static void append_import_decls(struct import_decl*** tail, struct import_decl* head) {
    if (head == NULL) {
        return;
    }

    **tail = head;
    while (**tail != NULL) {
        *tail = &((**tail)->next);
    }
}

static void append_export_decls(struct export_decl*** tail, struct export_decl* head) {
    if (head == NULL) {
        return;
    }

    **tail = head;
    while (**tail != NULL) {
        *tail = &((**tail)->next);
    }
}

static void append_type_decls(struct type_decl*** tail, struct type_decl* head) {
    if (head == NULL) {
        return;
    }

    **tail = head;
    while (**tail != NULL) {
        *tail = &((**tail)->next);
    }
}

static void append_var_decls(struct var_decl*** tail, struct var_decl* head) {
    if (head == NULL) {
        return;
    }

    **tail = head;
    while (**tail != NULL) {
        *tail = &((**tail)->next);
    }
}

static void append_fn_decls(struct fn_decl*** tail, struct fn_decl* head) {
    if (head == NULL) {
        return;
    }

    **tail = head;
    while (**tail != NULL) {
        *tail = &((**tail)->next);
    }
}

static int tokens_equal(const struct token* left, const struct token* right) {
    return left != NULL && right != NULL && left->length == right->length &&
           strncmp(left->start_pos, right->start_pos, (size_t)left->length) == 0;
}

static int mark_exported_name(struct program_ast* program, const struct token* name) {
    struct fn_decl* fn = program->fn_decls;
    while (fn != NULL) {
        if (tokens_equal(name, fn->name)) {
            fn->is_exported = 1;
            return 1;
        }
        fn = fn->next;
    }

    struct type_decl* type = program->type_decls;
    while (type != NULL) {
        if (tokens_equal(name, type->name)) {
            type->is_exported = 1;
            return 1;
        }
        type = type->next;
    }

    struct var_decl* var = program->var_decls;
    while (var != NULL) {
        if (tokens_equal(name, var->name)) {
            var->is_exported = 1;
            return 1;
        }
        var = var->next;
    }

    return 0;
}

void module_ast_builder_init(struct module_ast_builder* builder, char* module_name) {
    builder->program.module_name = NULL;
    builder->program.module_path = module_name;
    builder->program.imports = NULL;
    builder->program.exports = NULL;
    builder->program.type_decls = NULL;
    builder->program.var_decls = NULL;
    builder->program.fn_decls = NULL;

    builder->imports_tail = &builder->program.imports;
    builder->exports_tail = &builder->program.exports;
    builder->types_tail = &builder->program.type_decls;
    builder->vars_tail = &builder->program.var_decls;
    builder->fns_tail = &builder->program.fn_decls;
}

void module_ast_builder_append(struct module_ast_builder* builder, struct program_ast* file_program) {
    if (builder->program.module_name == NULL) {
        builder->program.module_name = file_program->module_name;
    }

    append_import_decls(&builder->imports_tail, file_program->imports);
    append_export_decls(&builder->exports_tail, file_program->exports);
    append_type_decls(&builder->types_tail, file_program->type_decls);
    append_var_decls(&builder->vars_tail, file_program->var_decls);
    append_fn_decls(&builder->fns_tail, file_program->fn_decls);
}

void module_ast_validate_exports(struct program_ast* program) {
    struct export_decl* export = program->exports;
    while (export != NULL) {
        if (!mark_exported_name(program, export->name)) {
            error("Exported name is not declared in module");
        }
        export = export->next;
    }
}
